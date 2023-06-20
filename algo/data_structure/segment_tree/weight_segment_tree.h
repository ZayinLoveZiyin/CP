template <typename T>
class weight_segment_tree {
 public:
  weight_segment_tree(int n_) : rt(-1) { n = n_; }

  void add(int& k, int l, int r, int a, int b, T d) {
    if (!~k) k = newnode();
    if (a > b || l > b || a > r) return;
    if (a <= l && r <= b) {
      tag[k] += d;
      return;
    }
    int mid = (l + r) >> 1;
    add(lson[k], l, mid, a, b, d);
    add(rson[k], mid + 1, r, a, b, d);
    sum[k] = tags[k] * (r - l + 1);
    if (~lson[k]) sum[k] += sum[lson[k]];
    if (~rson[k]) sum[k] += sum[rson[k]];
  }

  inline void add(int l, int r, T d) {
    assert(0 <= l && l < n);
    assert(0 <= r && r < n);
    add(rt, 0, n - 1, l, r, d);
  }

  T query(int k, int l, int r, int a, int b) {
    assert(l <= a && b <= r);
    if (a == l && r == b) return sum[k];
    int mid = (l + r) >> 1;
    T res = tag[k] * (b - a + 1);
    if (b <= mid)
      res += query(k << 1, l, mid, a, b);
    else if (a > mid)
      res += query(k << 1 | 1, mid + 1, r, a, b);
    else
      res += query(k << 1, l, mid, a, mid) +
             query(k << 1 | 1, mid + 1, r, mid + 1, b);
    return res;
  }

  inline T query(int l, int r) {
    if (l > r) return 0;
    assert(0 <= l && l < n);
    assert(0 <= r && r < n);
    return query(1, 0, n - 1, l, r);
  }

 private:
  int newnode() {
    int k = sum.size();
    tag.push_back(0);
    sum.push_back(0);
    lson.push_back(-1);
    rson.push_back(-1);
    return k;
  }

  int n, rt;
  std::vector<int> lson, rson;
  std::vector<T> tag, sum;
};