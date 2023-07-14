namespace rolling_hash {

template <long long P, long long B>
struct mutable_rolling_hash {
  using mint = modular::mint<P>;

  mutable_rolling_hash(int n_) : a(n_), tree(n_), pwB(n_), invpwB(n_) {
    pwB[0] = 1;
    for (int i = 1; i < n_; ++i) pwB[i] = pwB[i - 1] * B;

    mint invB = mint(B).inv();
    invpwB[0] = 1;
    for (int i = 1; i < n_; ++i) invpwB[i] = invpwB[i - 1] * invB;
  }

  void modify(int i, int x) {
    tree.add(i, mint(x - a[i]) * pwB[i]);
    a[i] = x;
  }

  mint get_hash(int l, int r) { return tree.range_sum(l, r) * invpwB[l]; }

  std::vector<int> a;
  std::vector<mint> pwB, invpwB;
  fenwick_tree<mint> tree;
};

}  // namespace rolling_hash

using mutable_rolling_hash =
    rolling_hash::mutable_rolling_hash<(long long)1e18 + 9, (long long)1e9 + 7>;