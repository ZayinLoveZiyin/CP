struct info {
  info() {}
  friend info operator+(const info& lhs, const info& rhs) { return info(); }

  friend std::ostream& operator<<(std::ostream& os, const info& v) {
    return os;
  }
};

template <typename T>
class segment_tree {
 public:
  segment_tree(int n_, const T& t = T()) : raw_n(n_) {
    for (n = 1; n < n_; n <<= 1) {
    }
    tree.resize(n << 1, t);
  }
  template <typename U>
  segment_tree(
      std::vector<U> a,
      std::function<T(U)> trans = [](const U& u) { return T(u); })
      : segment_tree(a.size()) {
    for (int i = 0; i < a.size(); ++i) tree[n + i] = trans(a[i]);
    for (int i = n; --i;) tree[i] = tree[i << 1] + tree[i << 1 | 1];
  }

  void change(int i, const T& t) {
    assert(0 <= i && i < raw_n);
    tree[i += n] = t;
    while (i >>= 1) tree[i] = tree[i << 1] + tree[i << 1 | 1];
  }

  // [l,r]
  T composite(int l, int r) {
    if (l > r) return T();
    assert(0 <= l && l < raw_n);
    assert(0 <= r && r < raw_n);
    T prodL, prodR;
    for (l += n, r += n + 1; l < r; l >>= 1, r >>= 1) {
      if (l & 1) prodL = prodL + tree[l++];
      if (r & 1) prodR = tree[--r] + prodR;
    }
    return prodL + prodR;
  }

  // find max l s.t. predicate(composite(l,r)) returns true,
  // should guarantee that for all p<=l, predicate(composite(p,r)) return true
  // returns -1 if there is no such l.
  int find_left(int r, std::function<bool(const info&)> predicate) {
    assert(0 <= r && r < raw_n);
    info cur_sum;
    if (predicate(cur_sum)) return r + 1;
    for (r += n + 1;; r >>= 1)
      if ((r & 1) || r == 2) {
        if (predicate(tree[r - 1] + cur_sum)) {
          for (; r <= n;)
            if (!predicate(tree[(r <<= 1) - 1] + cur_sum)) {
              cur_sum = tree[--r] + cur_sum;
            }
          return r - n - 1;
        }
        --r;
        if (!(r & (r - 1))) return -1;
      }
  }

  // find min r s.t. predicate(composite(l,r)) returns true,
  // should guarantee that for all p>=r, predicate(composite(l,p)) return true
  // returns n if there is no such r.
  int find_right(int l, std::function<bool(const info&)> predicate) {
    assert(0 <= l && l < raw_n);
    info cur_sum;
    if (predicate(cur_sum)) return l - 1;
    for (l += n;; l >>= 1)
      if (l & 1) {
        if (predicate(cur_sum + tree[l])) {
          for (; l < n;)
            if (!predicate(cur_sum + tree[l <<= 1])) {
              cur_sum + cur_sum + tree[l++];
            }
          return l - n;
        }
        ++l;
        if (!(l & (l - 1))) return raw_n;
      }
  }

 private:
  int raw_n, n;
  std::vector<T> tree;
};