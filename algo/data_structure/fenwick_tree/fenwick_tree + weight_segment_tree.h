template <typename T>
class weight_segment_tree {
 public:
  weight_segment_tree(int n_) : rt(-1) { n = n_; }

  int add(int k, int l, int r, int p, T d) {
    if (p < l || p > r) return k;
    if (!~k) k = newnode();
    sum[k] = sum[k] + d;
    if (l == r) return k;
    int mid = (l + r) >> 1;
    lson[k] = add(lson[k], l, mid, p, d);
    rson[k] = add(rson[k], mid + 1, r, p, d);
    return k;
  }

  inline void add(int p, T d) {
    assert(0 <= p && p < n);
    rt = add(rt, 0, n - 1, p, d);
  }

  T query(int k, int l, int r, int a, int b) {
    if (!~k || a > b || l > b || a > r) return T();
    if (a <= l && r <= b) return sum[k];
    int mid = (l + r) >> 1;
    return query(lson[k], l, mid, a, b) + query(rson[k], mid + 1, r, a, b);
  }

  inline T query(int l, int r) {
    if (l > r) return T();
    assert(0 <= l && l < n);
    assert(0 <= r && r < n);
    return query(rt, 0, n - 1, l, r);
  }

 private:
  int newnode() {
    int k = sum.size();
    sum.push_back(T());
    lson.push_back(-1);
    rson.push_back(-1);
    return k;
  }

  int n, rt;
  std::vector<int> lson, rson;
  std::vector<T> sum;
};

// 0-index based
template <typename T>
class fenwick_tree_2d {
 public:
  fenwick_tree_2d(int n_, int m_)
      : n(n_), tree(n_, weight_segment_tree<T>(m_)) {}

  void add(int x, int y, T d) {
    for (int i = x + 1; i <= n; i += i & -i) {
      tree[i - 1].add(y, d);
    }
  }

  T prefix_sum(int x, int ly, int ry) {
    auto ans = T();
    for (int i = x + 1; i > 0; i -= i & -i) {
      T t = tree[i - 1].query(ly, ry);
      ans = ans + t;
    }
    return ans;
  }

  T range_sum(int lx, int rx, int ly, int ry) {
    T tr = prefix_sum(rx, ly, ry);
    T tl = prefix_sum(lx - 1, ly, ry);
    return tr - tl;
  }

 private:
  int n, m;
  std::vector<weight_segment_tree<T>> tree;
};