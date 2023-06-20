// 0-index based
template <typename T>
class fenwick_tree {
 public:
  fenwick_tree(int n_) : n(n_), tree(n) {}

  void add(int x, T v) {
    for (int i = x + 1; i <= n; i += i & -i) {
      tree[i - 1] += v;
    }
  }

  T prefix_sum(int x) {
    auto ans = T();
    for (int i = x + 1; i > 0; i -= i & -i) ans += tree[i - 1];
    return ans;
  }

  T range_sum(int l, int r) { return prefix_sum(r) - prefix_sum(l - 1); }

 private:
  int n;
  std::vector<T> tree;
};