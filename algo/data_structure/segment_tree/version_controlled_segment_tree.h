struct LazyTag {
  int d;
  LazyTag(int d = 0) : d(d) {}

  LazyTag& operator+=(const LazyTag& t) {
    d += t.d;
    return *this;
  }

  friend std::ostream& operator<<(std::ostream& os, const LazyTag& t) {
    return os << t.d;
  }

  operator bool() const { return d; }
};

struct LazyInfo {
  int v;
  LazyInfo() : LazyInfo(0) {}
  LazyInfo(int v) : v(v) {}

  friend LazyInfo operator+(const LazyInfo& lhs, const LazyInfo& rhs) {
    return LazyInfo(lhs.v + rhs.v);
  }

  LazyInfo& operator+=(const LazyTag& t) {
    v += t.d;
    return *this;
  }

  friend std::ostream& operator<<(std::ostream& os, const LazyInfo& info) {
    return os << info.v;
  }
};

template <typename Info, typename Tag>
class VersionControlledSegmentTree {
 private:
  struct node;

 public:
  using version_handle = node*;
  VersionControlledSegmentTree(int n_, const Info& v = {})
      : VersionControlledSegmentTree(std::vector<Info>(n_, v)) {}
  VersionControlledSegmentTree(const std::vector<Info>& a) : n(a.size()) {
    std::function<node*(int, int)> build = [&](int l, int r) {
      node* rt;
      if (l == r) {
        rt = new_node(a[l]);
      } else {
        int mid = (l + r) >> 1;
        rt = new_node(build(l, mid), build(mid + 1, r));
      }
      return rt;
    };
    rt = build(0, n - 1);
  }

  node* current_version() { return rt; }

  node* change(node* rt, int l, int r, int p, const Info& v) {
    rt = copy_node(rt);
    if (l == r) return new_node(v);
    push(rt);
    int mid = (l + r) >> 1;
    if (p <= mid)
      rt->ls = change(rt->ls, l, mid, p, v);
    else
      rt->rs = change(rt->rs, mid + 1, r, p, v);
    pull(rt);
    return rt;
  }

  inline node* change(node* rt, int p, const Info& v) {
    assert(0 <= p && p < n);
    return change(rt, 0, n - 1, p, v);
  }

  inline node* change(int p, const Info& v) {
    assert(0 <= p && p < n);
    return rt = change(rt, p, v);
  }

  node* apply(node* rt, int l, int r, int a, int b, const Tag& t) {
    if (a > b || l > b || a > r) return rt;
    rt = copy_node(rt);
    if (a <= l && r <= b) {
      apply(rt, t);
    } else {
      push(rt);
      int mid = (l + r) >> 1;
      rt->ls = apply(rt->ls, l, mid, a, b, t);
      rt->rs = apply(rt->rs, mid + 1, r, a, b, t);
      pull(rt);
    }
    return rt;
  }

  inline node* apply(node* rt, int l, int r, const Tag& t) {
    return apply(rt, 0, n - 1, l, r, t);
  }

  inline node* apply(int l, int r, const Tag& t) {
    return rt = apply(rt, l, r, t);
  }

  Info composite(node* rt, int l, int r, int a, int b) {
    if (l > b || a > r) return Info();
    if (a <= l && r <= b) return rt->info;
    push(rt);
    int mid = (l + r) >> 1;
    if (b <= mid) return composite(rt->ls, l, mid, a, b);
    if (a > mid) return composite(rt->rs, mid + 1, r, a, b);
    return composite(rt->ls, l, mid, a, b) +
           composite(rt->rs, mid + 1, r, a, b);
  }

  inline Info composite(node* rt, int l, int r) {
    if (l > r) return Info();
    assert(0 <= l && l < n);
    assert(0 <= r && r < n);
    return composite(rt, 0, n - 1, l, r);
  }

  inline Info composite(int l, int r) {
    if (l > r) return Info();
    assert(0 <= l && l < n);
    assert(0 <= r && r < n);
    return composite(rt, 0, n - 1, l, r);
  }

  std::vector<Info> flat(node* rt) {
    std::vector<Info> res;
    std::function<void(node*)> travel = [&](node* rt) {
      if (!~rt->ls || !~rt->rs) {
        res.push_back(rt->info);
      } else {
        push(rt);
        travel(rt->ls);
        travel(rt->rs);
      }
    };
    travel(rt);
    return res;
  }

  // dest[a,b]=source[a,b]
  node* copy(node* dest, node* source, int a, int b) {
    std::function<node*(node*, node*, int, int)> travel = [&](node* x, node* y,
                                                              int l, int r) {
      if (a > r || l > b) return x;
      if (a <= l && r <= b) return y;
      push(x);
      push(y);
      int mid = (l + r) >> 1;
      return new_node(travel(x->ls, y->ls, l, mid),
                      travel(x->rs, y->rs, mid + 1, r));
    };
    return travel(dest, source, 0, n - 1);
  }

  node* copy(node* source, int a, int b) { return rt = copy(rt, source, a, b); }

  // return lhs<rhs
  bool compare(int lhs, int rhs) {
    std::function<bool(node*, node*, int, int)> travel =
        [&](node* lhs, node* rhs, int l, int r) {
          if (lhs->info == rhs.info) return false;
          if (l == r) return lhs->info < rhs->info;
          int mid = (l + r) >> 1;
          push(lhs);
          push(rhs);
          if (lhs->ls->info == rhs->ls->info)
            return travel(lhs->rs, rhs->rs, mid + 1, r);
          return travel(lhs->ls, rhs->ls, l, mid);
        };
    return travel(lhs, rhs, 0, n - 1);
  }

 private:
  int n;
  node* rt;
  struct node {
    Info info;
    Tag tag;
    node *ls = nullptr, *rs = nullptr;

    friend std::ostream& operator<<(std::ostream& os, const node& nd) {}
  };

  inline node* new_node(node nd = {}) { return new node(std::move(nd)); }
  inline node* new_node(const Info& v) {
    return new_node(node{
        .info = v,
    });
  }
  inline node* new_node(node* ls, node* rs) {
    return new_node(node{
        .info = ls->info + rs->info,
        .tag = {},
        .ls = ls,
        .rs = rs,
    });
  }

  inline node* copy_node(node* rt) { return new_node(*rt); }

  inline void push(node* rt) {
    if (!rt->tag) return;
    rt->ls = copy_node(rt->ls);
    apply(rt->ls, rt->tag);
    rt->rs = copy_node(rt->rs);
    apply(rt->rs, rt->tag);
    rt->tag = Tag();
  }
  inline void pull(node* rt) { rt->info = rt->ls->info + rt->rs->info; }
  inline void apply(node* rt, const Tag& t) {
    rt->info += t;
    rt->tag += t;
  }
};