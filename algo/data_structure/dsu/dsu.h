class disjoint_set_union {
 public:
  disjoint_set_union(int n_) : n(n_), fa(n_, -1), sz(n_, 1) {}

  int find(int i) { return !~fa[i] ? i : fa[i] = find(fa[i]); }

  bool is_in_same_set(int u, int v) { return find(u) == find(v); }

  int get_size(int u) { return sz[find(u)]; }

  bool merge(int u, int v) {
    int fu = find(u), fv = find(v);
    if (fu == fv) return false;
    sz[fu] += sz[fv];
    fa[fv] = fu;
    return true;
  }

 private:
  const int n;
  std::vector<int> fa;
  std::vector<int> sz;
};