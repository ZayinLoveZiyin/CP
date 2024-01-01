struct LazyTag {
  LazyTag() {}
  friend std::ostream& operator<<(std::ostream& os, const LazyTag& t) {}
};

struct LazyInfo {
  LazyInfo() {}

  friend LazyInfo operator+(const LazyInfo& lhs, const LazyInfo& rhs) {}

  friend std::ostream& operator<<(std::ostream& os, const LazyInfo& info) {
    return os;
  }

  void apply(const LazyTag& t) {}
};

template <typename Info, typename Tag>
class VersionControlledSegmentTree {
 private:
  int n;
  struct node {
    Info info;
    Tag tag;
    std::shared_ptr<node> ls, rs;

    node(const Info& v = {}) : info(v) {}

    static std::shared_ptr<node> copy(const std::shared_ptr<node>& rt) {
      return std::make_shared<node>(*rt);
    }

    static void duplicate(std::shared_ptr<node>& rt) {
      if (rt.use_count() == 1) return;
      rt = copy(rt);
    }

    inline void push() {
      if (!tag.delta) return;
      duplicate(ls);
      ls->apply(tag);
      duplicate(rs);
      rs->apply(tag);
      tag = Tag();
    }
    inline void pull() { info = ls->info + rs->info; }
    inline void apply(const Tag& t) {
      info.apply(t);
      tag.apply(t);
    }

    std::vector<Info> dump() {
      std::vector<Info> res;
      std::function<void(node*)> travel = [&](node* rt) {
        if (!rt->ls || !rt->rs) {
          res.push_back(rt->info);
        } else {
          rt->push();
          travel(rt->ls.get());
          travel(rt->rs.get());
        }
      };
      travel(this);
      return res;
    }
  };

  std::shared_ptr<node> rt;

 public:
  VersionControlledSegmentTree(int n_, const Info& v = {})
      : VersionControlledSegmentTree(std::vector<Info>(n_, v)) {}
  VersionControlledSegmentTree(const std::vector<Info>& a) : n(a.size()) {
    std::function<std::shared_ptr<node>(int, int)> build = [&](int l, int r) {
      auto rt = std::make_shared<node>();
      if (l == r) {
        rt->info = a[l];
      } else {
        int mid = (l + r) >> 1;
        rt->ls = build(l, mid);
        rt->rs = build(mid + 1, r);
        rt->pull();
      }
      return rt;
    };
    rt = build(0, n - 1);
  }

  std::shared_ptr<node> current_root() { return node::copy(rt); }

  std::vector<Info> dump() { return rt->dump(); }

  void change(std::shared_ptr<node>& rt, int l, int r, int p, const Info& v) {
    node::duplicate(rt);
    if (l == r) {
      rt = std::make_shared<node>(v);
      return;
    }
    rt->push();
    int mid = (l + r) >> 1;
    if (p <= mid)
      change(rt->ls, l, mid, p, v);
    else
      change(rt->rs, mid + 1, r, p, v);
    rt->pull();
  }

  inline void change(int p, const Info& v) {
    assert(0 <= p && p < n);
    change(rt, 0, n - 1, p, v);
  }

  void apply(std::shared_ptr<node>& rt, int l, int r, int a, int b,
             const Tag& t) {
    if (a > b || l > b || a > r) return;
    node::duplicate(rt);
    if (a <= l && r <= b) {
      rt->apply(t);
    } else {
      rt->push();
      int mid = (l + r) >> 1;
      apply(rt->ls, l, mid, a, b, t);
      apply(rt->rs, mid + 1, r, a, b, t);
      rt->pull();
    }
  }

  inline void apply(int l, int r, const Tag& t) {
    apply(rt, 0, n - 1, l, r, t);
  }

  Info composite(const std::shared_ptr<node>& rt, int l, int r, int a, int b) {
    if (l > b || a > r) return Info();
    if (a <= l && r <= b) return rt->info;
    rt->push();
    int mid = (l + r) >> 1;
    if (b <= mid) return composite(rt->ls, l, mid, a, b);
    if (a > mid) return composite(rt->rs, mid + 1, r, a, b);
    return composite(rt->ls, l, mid, a, b) +
           composite(rt->rs, mid + 1, r, a, b);
  }

  inline Info composite(int l, int r) {
    if (l > r) return Info();
    assert(0 <= l && l < n);
    assert(0 <= r && r < n);
    return composite(rt, 0, n - 1, l, r);
  }

  // return lhs<rhs
  bool compare(const std::shared_ptr<node>& lhs,
               const std::shared_ptr<node>& rhs) {
    std::function<bool(const std::shared_ptr<node>&,
                       const std::shared_ptr<node>&, int, int)>
        travel = [&](const std::shared_ptr<node>& lhs,
                     const std::shared_ptr<node>& rhs, int l, int r) {
          if (lhs->info == rhs->info) return false;
          if (l == r) return lhs->info < rhs->info;
          int mid = (l + r) >> 1;
          lhs->push();
          rhs->push();
          if (lhs->ls->info == rhs->ls->info)
            return travel(lhs->rs, rhs->rs, mid + 1, r);
          return travel(lhs->ls, rhs->ls, l, mid);
        };
    return travel(lhs, rhs, 0, n - 1);
  }
};
