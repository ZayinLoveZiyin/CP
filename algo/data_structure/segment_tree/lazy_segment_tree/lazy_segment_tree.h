struct lazy_tag {
  void apply(const lazy_tag& t) {}
  friend std::ostream& operator<<(std::ostream& os, const lazy_tag& t) {}
};

struct lazy_info {
  friend lazy_info operator+(const lazy_info& lhs, const lazy_info& rhs) {}

  void apply(const lazy_tag& t) {}

  friend std::ostream& operator<<(std::ostream& os, const lazy_info& t) {}
};

template <typename Info, typename Tag>
class lazy_segment_tree {
 public:
  lazy_segment_tree(int n_) {
    n = n_;
    infos.resize(4 << std::__lg(n));
    tags.resize(4 << std::__lg(n));
  }
  lazy_segment_tree(int n_, const Info& v)
      : lazy_segment_tree(std::vector<Info>(n_, v)) {}
  lazy_segment_tree(const std::vector<Info>& a) : lazy_segment_tree(a.size()) {
    std::function<void(int, int, int)> build = [&](int k, int l, int r) {
      if (l == r) {
        infos[k] = a[l];
        return;
      }
      int mid = (l + r) >> 1;
      build(k << 1, l, mid);
      build(k << 1 | 1, mid + 1, r);
      pull(k);
    };
    build(1, 0, n - 1);
  }

  void change(int k, int l, int r, int p, const Info& v) {
    if (l == r) {
      infos[k] = v;
      return;
    }
    push(k);
    int mid = (l + r) >> 1;
    if (p <= mid)
      change(k << 1, l, mid, p, v);
    else
      change(k << 1 | 1, mid + 1, r, p, v);
    pull(k);
  }

  inline void change(int p, const Info& v) {
    assert(0 <= p && p < n);
    change(1, 0, n - 1, p, v);
  }

  void apply(int k, int l, int r, int a, int b, const Tag& t) {
    if (a > b || l > b || a > r) return;
    if (a <= l && r <= b) {
      apply(k, t);
      return;
    }
    push(k);
    int mid = (l + r) >> 1;
    apply(k << 1, l, mid, a, b, t);
    apply(k << 1 | 1, mid + 1, r, a, b, t);
    pull(k);
  }

  inline void apply(int l, int r, const Tag& t) { apply(1, 0, n - 1, l, r, t); }

  Info composite(int k, int l, int r, int a, int b) {
    if (l > b || a > r) return Info();
    if (a <= l && r <= b) return infos[k];
    push(k);
    int mid = (l + r) >> 1;
    if (b <= mid) return composite(k << 1, l, mid, a, b);
    if (a > mid) return composite(k << 1 | 1, mid + 1, r, a, b);
    return composite(k << 1, l, mid, a, b) +
           composite(k << 1 | 1, mid + 1, r, a, b);
  }

  inline Info composite(int l, int r) {
    if (l > r) return Info();
    assert(0 <= l && l < n);
    assert(0 <= r && r < n);
    return composite(1, 0, n - 1, l, r);
  }

  std::vector<Info> flat(int a, int b) {
    std::vector<Info> res;
    std::function<void(int, int, int)> travel = [&](int k, int l, int r) {
      if (l > b || a > r) return;
      if (l == r) {
        res.push_back(infos[k]);
        return;
      }
      push(k);
      int mid = (l + r) >> 1;
      travel(k << 1, l, mid);
      travel(k << 1 | 1, mid + 1, r);
      pull(k);
    };
    travel(1, 0, n - 1);
    return res;
  }

  std::vector<Info> flat() { return flat(0, n - 1); }

 private:
  inline void push(int k) {
    apply(k << 1, tags[k]);
    apply(k << 1 | 1, tags[k]);
    tags[k] = Tag();
  }
  inline void pull(int k) { infos[k] = infos[k << 1] + infos[k << 1 | 1]; }

  inline void apply(int k, const Tag& t) {
    infos[k].apply(t);
    tags[k].apply(t);
  }

  int n;
  std::vector<Info> infos;
  std::vector<Tag> tags;
};
