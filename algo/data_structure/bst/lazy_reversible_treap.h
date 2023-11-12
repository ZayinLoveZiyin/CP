struct LazyTag {
  LazyTag() {}
  void apply(const LazyTag& t) { d += t.d; }
  friend std::ostream& operator<<(std::ostream& os, const LazyTag& t) {
    return os;
  }
};

struct LazyReversibleInfo {
  LazyReversibleInfo() {}
  friend LazyReversibleInfo operator+(const LazyReversibleInfo& lhs,
                                      const LazyReversibleInfo& rhs) {
    return LazyReversibleInfo();
  }

  void apply(const LazyTag& t) {}

  void reverse() {}

  friend std::ostream& operator<<(std::ostream& os,
                                  const LazyReversibleInfo& v) {
    return os;
  }
};

// (a+b)+c=a+(b+c)
// f(a+b)=f(a)+f(b)
template <typename LazyReversibleInfo, typename LazyTag>
class LazyReversibleTreap {
 public:
  struct Node {
    LazyTag tag;
    LazyReversibleInfo val, sum;
    using priority_type = unsigned int;
    priority_type priority;
    int size;
    bool is_rev;
    Node *lson, *rson;
    static priority_type get_priority() {
      static std::mt19937_64 engine(
          std::chrono::steady_clock::now().time_since_epoch().count());
      static std::uniform_int_distribution<priority_type> rng(
          0, std::numeric_limits<priority_type>::max());
      return rng(engine);
    }

    Node(LazyReversibleInfo v)
        : val(v),
          sum(v),
          priority(get_priority()),
          size(1),
          is_rev(false),
          lson(nullptr),
          rson(nullptr) {}

    static int getSize(Node* x) { return x ? x->size : 0; }
    static LazyReversibleInfo getVal(Node* x) {
      return x ? x->val : LazyReversibleInfo();
    }
    static LazyReversibleInfo getSum(Node* x) {
      return x ? x->sum : LazyReversibleInfo();
    }
  };

  LazyReversibleTreap(Node* rt = nullptr) : rt(rt) {}
  LazyReversibleTreap(LazyReversibleInfo v) : rt(new Node(v)) {}
  ~LazyReversibleTreap() {
    std::function<void(Node*)> delete_node = [&](Node* rt) {
      if (!rt) return;
      delete_node(rt->lson);
      delete_node(rt->rson);
      delete rt;
    };
    delete_node(rt);
  }

  LazyReversibleTreap& operator=(const LazyReversibleTreap& trp) = delete;
  LazyReversibleTreap(const LazyReversibleTreap& trp) = delete;
  LazyReversibleTreap(LazyReversibleTreap&& trp) : rt(trp.rt) {
    trp.rt = nullptr;
  }
  LazyReversibleTreap& operator=(LazyReversibleTreap&& trp) {
    rt = trp.rt;
    trp.rt = nullptr;
    return *this;
  }

  int size() const { return Node::getSize(rt); }

  // return [k,n), and current treap remains [0,k)
  LazyReversibleTreap extract(int k) {
    if (k >= Node::getSize(rt)) return LazyReversibleTreap();
    Node* new_rt;
    splitBySize(rt, rt, new_rt, k);
    return LazyReversibleTreap(new_rt);
  }

  // append all info in trp, and trp will become empty
  LazyReversibleTreap& append(LazyReversibleTreap& trp) {
    rt = merge(rt, trp.rt);
    trp.rt = nullptr;
    return *this;
  }

  LazyReversibleTreap& insert(int k, LazyReversibleInfo v) {
    LazyReversibleTreap middle(v);
    LazyReversibleTreap suffix = extract(k);
    append(middle).append(suffix);
    return *this;
  }

  LazyReversibleTreap& erase(int l, int r) {
    auto [middle, suffix] = split3(l, r + 1);
    append(suffix);
    return *this;
  }

  LazyReversibleTreap& reverse() {
    reverse(rt);
    return *this;
  }
  LazyReversibleTreap& reverse(int l, int r) {
    auto [middle, suffix] = split3(l, r + 1);
    middle.reverse();
    append(middle).append(suffix);
    return *this;
  }

  LazyReversibleTreap& apply(LazyTag t) {
    apply(rt, t);
    return *this;
  }
  LazyReversibleTreap& apply(int l, int r, LazyTag t) {
    auto [middle, suffix] = split3(l, r + 1);
    middle.apply(t);
    append(middle).append(suffix);
    return *this;
  }

  LazyReversibleInfo sum() const { return Node::getSum(rt); }
  // [l,r]
  LazyReversibleInfo sum(int l, int r) {
    if (l > r) return LazyReversibleInfo();
    auto [middle, suffix] = split3(l, r + 1);
    auto res = middle.sum();
    append(middle).append(suffix);
    return res;
  }

  LazyReversibleInfo at(int k) const {
    assert(k < Node::getSize(rt));
    for (Node* cur = rt;;) {
      push(cur);
      int size_with_lson = Node::getSize(cur->lson);
      dbg(Node::getSize(cur), cur, cur->lson, cur->rson, size_with_lson, k);
      if (k == size_with_lson) return cur->val;
      if (k < size_with_lson) {
        cur = cur->lson;
      } else {
        k -= size_with_lson + 1;
        cur = cur->rson;
      }
    }
  }

  LazyReversibleTreap& change(int k, LazyReversibleInfo v) {
    assert(k < Node::getSize(rt));
    std::function<void(Node*, int)> change = [&](Node* rt, int k) {
      push(rt);
      int size_with_lson = Node::getSize(rt->lson);
      if (k == size_with_lson) {
        rt->val = v;
      } else if (k < size_with_lson) {
        change(rt->lson, k);
      } else {
        change(rt->rson, k - size_with_lson - 1);
      }
      pull(rt);
    };
    change(k, v);
    return *this;
  }

  std::vector<LazyReversibleInfo> flat() {
    std::vector<LazyReversibleInfo> res;
    std::function<void(Node*)> travel = [&](Node* rt) {
      if (!rt) return;
      push(rt);
      travel(rt->lson);
      res.push_back(rt->val);
      travel(rt->rson);
    };
    travel(rt);
    return res;
  }

  std::string to_string() const {
    std::ostringstream os;
    os << "{\n";
    std::function<void(Node*)> travel = [&](Node* rt) {
      if (!rt) return;
      push(rt);
      os << " " << rt << ": (" << rt->lson << "," << rt->rson << ") "
         << rt->size << " " << rt->val << "\n";
      travel(rt->lson);
      travel(rt->rson);
    };
    travel(rt);
    os << "}";
    return os.str();
  }

  // find max p s.t. f(p)=predicate(sum(p,size-1)) returns true,
  // should guarantee that for all k<=p, f(k) return true
  // returns -1 if there is no such p.
  int findLast(std::function<bool(const LazyReversibleInfo&)> predicate) {
    if (!predicate(Node::getSum(rt))) return -1;
    int res = Node::getSize(rt);
    LazyReversibleInfo suffix_sum = LazyReversibleInfo();
    for (Node* cur = rt;;) {
      if (predicate(suffix_sum)) return res;
      push(cur);
      if (predicate(Node::getSum(cur->rson) + suffix_sum)) {
        cur = cur->rson;
      } else {
        suffix_sum = cur->val + Node::getSum(cur->rson) + suffix_sum;
        res -= Node::getSize(cur->rson) + 1;
        cur = cur->lson;
      }
    }
  }

  // find min p s.t. f(p)=predicate(sum(0,p)) returns true,
  // should guarantee that for all k>=p, f(k) return true
  // returns size() if there is no such p.
  int findFirst(std::function<bool(const LazyReversibleInfo&)> predicate) {
    if (!predicate(Node::getSum(rt))) return Node::getSize(rt);
    int res = -1;
    LazyReversibleInfo prefix_sum = LazyReversibleInfo();
    for (Node* cur = rt;;) {
      if (predicate(prefix_sum)) return res;
      push(cur);
      if (predicate(prefix_sum + Node::getSum(cur->lson))) {
        cur = cur->lson;
      } else {
        prefix_sum = prefix_sum + Node::getSum(cur->lson) + cur->val;
        res += Node::getSize(cur->lson) + 1;
        cur = cur->rson;
      }
    }
  }

  int findRight(int k,
                std::function<bool(const LazyReversibleInfo&)> predicate) {
    assert(k < Node::getSize(rt));
    auto suffix = extract(k);
    int res = suffix.findFirst(predicate);
    append(suffix);
    return res;
  }

  int findLeft(int k,
               std::function<bool(const LazyReversibleInfo&)> predicate) {
    assert(k < Node::getSize(rt));
    auto suffix = extract(k + 1);
    int res = findLast(predicate);
    append(suffix);
    return res;
  }

 private:
  // return [l,r), [r,n)
  std::pair<LazyReversibleTreap, LazyReversibleTreap> split3(int l, int r) {
    assert(l <= r);
    LazyReversibleTreap middle = extract(l);
    LazyReversibleTreap suffix = middle.extract(r - l);
    return std::make_pair(std::move(middle), std::move(suffix));
  }

  static Node* reverse(Node* rt) {
    if (!rt) return nullptr;
    rt->is_rev ^= true;
    rt->val.reverse();
    rt->sum.reverse();
    return rt;
  }

  static Node* apply(Node* rt, LazyTag t) {
    if (!rt) return nullptr;
    rt->val.apply(t);
    rt->sum.apply(t);
    rt->tag.apply(t);
    return rt;
  }

  static Node* push(Node* rt) {
    if (!rt) return nullptr;
    if (rt->is_rev) {
      std::swap(rt->lson, rt->rson);
      reverse(rt->lson);
      reverse(rt->rson);
      rt->is_rev = false;
    }
    apply(rt->lson, rt->tag);
    apply(rt->rson, rt->tag);
    rt->tag = LazyTag();
    return rt;
  }

  static Node* pull(Node* rt) {
    if (!rt) return nullptr;
    rt->size = Node::getSize(rt->lson) + 1 + Node::getSize(rt->rson);
    rt->sum = Node::getSum(rt->lson) + rt->val + Node::getSum(rt->rson);
    return rt;
  }

  static Node* merge(Node* x, Node* y) {
    if (!x) return y;
    if (!y) return x;
    push(x);
    push(y);
    if (x->priority > y->priority) {
      x->rson = merge(x->rson, y);
      return pull(x);
    } else {
      y->lson = merge(x, y->lson);
      return pull(y);
    }
  }

  // split such that x->size=size
  static void splitBySize(Node* rt, Node*& x, Node*& y, int size) {
    if (!rt) {
      x = y = nullptr;
      return;
    }

    push(rt);
    int size_with_lson = Node::getSize(rt->lson) + 1;
    if (size_with_lson <= size) {
      x = rt;
      splitBySize(rt->rson, x->rson, y, size - size_with_lson);
    } else {
      y = rt;
      splitBySize(rt->lson, x, y->lson, size);
    }
    pull(rt);
  }

  Node* rt;
};

using Treap = LazyReversibleTreap<LazyReversibleInfo, LazyTag>;