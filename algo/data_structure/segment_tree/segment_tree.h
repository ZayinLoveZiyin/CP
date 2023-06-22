struct info {
  friend info operator+(const info& lhs, const info& rhs) {}

  friend std::ostream& operator<<(std::ostream& os, const info& v) {}
};

template <typename T>
class segment_tree {
 public:
  segment_tree(int n_) {
    for (n = 1; n < n_; n <<= 1) {
    }
    tree.resize(n << 1);
  }
  template <typename U>
  segment_tree(
      std::vector<U> a, auto trans = [](const U& u) { return T(u); })
      : segment_tree(a.size()) {
    for (int i = 0; i < a.size(); ++i) tree[n + i] = trans(a[i]);
    for (int i = n; --i;) tree[i] = tree[i << 1] + tree[i << 1 | 1];
  }

  void change(int i, const T& t) {
    assert(0 <= i && i < n);
    tree[i += n] = t;
    while (i >>= 1) tree[i] = tree[i << 1] + tree[i << 1 | 1];
  }

  // [l,r]
  T composite(int l, int r) {
    assert(0 <= l && l < n);
    assert(0 <= r && r < n);
    if (l > r) return T();
    T prodL, prodR;
    for (l += n, r += n + 1; l < r; l >>= 1, r >>= 1) {
      if (l & 1) prodL = prodL + tree[l++];
      if (r & 1) prodR = tree[--r] + prodR;
    }
    return prodL + prodR;
  }

 private:
  int n;
  std::vector<T> tree;
};