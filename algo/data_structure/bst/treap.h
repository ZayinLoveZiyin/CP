
struct Info {
  int x, val;
  Info(int x_, int v_ = 0) : x(x_), val(v_) {}
  friend bool operator<(const Info& lhs, const Info& rhs) {
    return lhs.x < rhs.x;
  }
  friend bool operator==(const Info& lhs, const Info& rhs) {
    return lhs.x == rhs.x;
  }
  friend bool operator<=(const Info& lhs, const Info& rhs) {
    return lhs.x <= rhs.x;
  }
  friend std::ostream& operator<<(std::ostream& os, const Info& info) {
    return os << "Info(" << info.x << ")";
  }
};

struct AccumulatedInfo {
  int sum;
  AccumulatedInfo(Info info) : sum(info.val){};
  AccumulatedInfo(long long s = 0) : sum(s){};
  friend AccumulatedInfo operator+(const AccumulatedInfo& lhs,
                                   const AccumulatedInfo& rhs) {
    return AccumulatedInfo(lhs.sum + rhs.sum);
  }
  friend std::ostream& operator<<(std::ostream& os,
                                  const AccumulatedInfo& accumulated_info) {
    return os << "AccumulatedInfo(" << accumulated_info.sum << ")";
  }
};

template <typename Info, typename AccumulatedInfo>
class treap {
 public:
  struct node {
    Info info;
    AccumulatedInfo accumulated_info;
    using priority_type = unsigned int;
    priority_type priority;
    size_t size;
    node *lson, *rson;
    static priority_type get_priority() {
      static std::mt19937_64 engine(
          std::chrono::steady_clock::now().time_since_epoch().count());
      static std::uniform_int_distribution<priority_type> rng(
          0, std::numeric_limits<priority_type>::max());
      return rng(engine);
    }

    node(Info info_)
        : info(info_),
          accumulated_info(info_),
          priority(get_priority()),
          size(1),
          lson(nullptr),
          rson(nullptr) {}

    node* maintain() {
      accumulated_info = AccumulatedInfo(info);
      size = 1;
      if (lson) {
        accumulated_info = lson->accumulated_info + accumulated_info;
        size += lson->size;
      }
      if (rson) {
        accumulated_info = accumulated_info + rson->accumulated_info;
        size += rson->size;
      }
      return this;
    }

    static size_t get_size(node* x) {
      if (!x) return 0;
      return x->size;
    }
  };

  struct iterator {
    treap* tree;
    int rank;
    iterator(treap* tree_, int rank_) : tree(tree_), rank(rank_) {
      assert(0 <= rank && rank <= node::get_size(tree->rt));
    }
    iterator& operator++() { return *this = iterator(tree, rank + 1); }
    iterator operator++(int) {
      iterator res = *this;
      ++*this;
      return res;
    }
    iterator& operator--() { return *this = iterator(tree, rank - 1); }
    iterator operator--(int) {
      iterator res = *this;
      --*this;
      return res;
    }
    friend bool operator==(const iterator& lhs, const iterator& rhs) {
      return lhs.tree == rhs.tree && lhs.rank == rhs.rank;
    }
    friend bool operator!=(const iterator& lhs, const iterator& rhs) {
      return lhs.tree != rhs.tree || lhs.rank != rhs.rank;
    }

    int get_rank() { return rank; }
    Info info() { return treap::get_kth(tree->rt, rank)->info; }
    AccumulatedInfo prefix_accumulated_info() {
      AccumulatedInfo res;
      auto handler = [&res](node* k) {
        if (k->lson) {
          res = res + k->lson->accumulated_info;
        }
        res = res + AccumulatedInfo(k->info);
      };
      node* nd = treap::get_kth(
          tree->rt, rank,
          /*go_left_handler=*/[&](node*) {},
          /*go_right_handler=*/handler);
      handler(nd);
      return res;
    }
    AccumulatedInfo suffix_accumulated_info() {
      AccumulatedInfo res;
      auto handler = [&res](node* k) {
        res = res + AccumulatedInfo(k->info);
        if (k->rson) {
          res = k->rson->accumulated_info + res;
        }
      };
      node* nd = treap::get_kth(tree->rt, rank, /*go_left_handler=*/handler);
      handler(nd);
      return res;
    }
  };

  treap(node* rt_ = nullptr) : rt(rt_) {}
  ~treap() {
    std::function<void(node*)> delete_node = [&](node* rt) {
      if (!rt) return;
      delete_node(rt->lson);
      delete_node(rt->rson);
      delete rt;
    };
    delete_node(rt);
  }

  iterator begin() { return iterator(this, 0); }
  iterator end() { return iterator(this, node::get_size(rt)); }

  iterator get_kth(int k) { return iterator(this, k); }

  // the first one >=v
  iterator lowerbound(Info v) {
    return iterator(this, get_count(rt, [&](Info x) { return x < v; }));
  }

  // the first one >v
  iterator upperbound(Info v) {
    return iterator(this, get_count(rt, [&](Info x) { return x <= v; }));
  }

  void insert(Info v) {
    node* mid = new node(v);
    node *l, *r;
    split_by_info(rt, l, r, [&](Info x) { return x < v; });
    rt = merge(merge(l, mid), r);
  }

  void erase(Info v, bool erase_all = false) {
    node *l, *mid, *r;
    split_by_info(rt, l, r, [&](Info x) { return x < v; });
    split_by_info(r, mid, r, [&](Info x) { return x <= v; });
    if (mid) {
      if (erase_all) {
        mid = nullptr;
      } else {
        mid = merge(mid->lson, mid->rson);
      }
    }
    rt = merge(merge(l, mid), r);
  }

  AccumulatedInfo inline lesum(Info v) {
    iterator it = upperbound(v);
    if (it == begin()) return AccumulatedInfo();
    --it;
    return it.prefix_accumulated_info();
  }
  AccumulatedInfo inline resum(Info v) {
    iterator it = lowerbound(v);
    if (it == end()) return AccumulatedInfo();
    return it.suffix_accumulated_info();
  }

  treap& operator=(const treap& tr) = delete;
  treap(const treap& tr) = delete;

 private:
  static node* merge(node* x, node* y) {
    if (!x) return y;
    if (!y) return x;
    if (x->priority > y->priority) {
      x->rson = merge(x->rson, y);
      return x->maintain();
    } else {
      y->lson = merge(x, y->lson);
      return y->maintain();
    }
  }

  // split such that predicate(all node in x)=true
  // precondition: if predicate(rt[i])=false, predicate(rt[j])=false for i<=j
  static void split_by_info(node* rt, node*& x, node*& y,
                            const std::function<bool(Info)>& predicate) {
    if (!rt) {
      x = nullptr;
      y = nullptr;
      return;
    }
    if (predicate(rt->info)) {
      x = rt;
      split_by_info(rt->rson, x->rson, y, predicate);
    } else {
      y = rt;
      split_by_info(rt->lson, x, y->lson, predicate);
    }
    rt->maintain();
  }

  // split such that x->size=size
  static void split_by_size(node* rt, node*& x, node*& y, size_t size) {
    if (!rt) {
      x = nullptr;
      y = nullptr;
      return;
    }

    size_t size_with_lson = node::get_size(rt->lson) + 1;
    if (size_with_lson <= size) {
      x = rt;
      split_by_size(rt->rson, x->rson, y, size - size_with_lson);
    } else {
      y = rt;
      split_by_size(rt->lson, x, y->lson, size);
    }
    rt->maintain();
  }

  static size_t get_count(node* rt,
                          const std::function<bool(Info)>& predicate) {
    size_t res = 0;
    for (; rt;) {
      if (predicate(rt->info)) {
        res += node::get_size(rt->lson) + 1;
        rt = rt->rson;
      } else {
        rt = rt->lson;
      }
    }
    return res;
  }

  // index from 0
  static node* get_kth(
      node* rt, int k,
      const std::function<void(node*)>& go_left_handler = [&](node*) {},
      const std::function<void(node*)>& go_right_handler = [&](node*) {}) {
    assert(0 <= k && k < rt->size);
    for (++k;;) {
      size_t size_with_lson = node::get_size(rt->lson) + 1;
      if (size_with_lson == k) return rt;
      if (size_with_lson <= k) {
        go_right_handler(rt);
        k -= size_with_lson;
        rt = rt->rson;
      } else {
        go_left_handler(rt);
        rt = rt->lson;
      }
    }
  }

  node* rt;
};
