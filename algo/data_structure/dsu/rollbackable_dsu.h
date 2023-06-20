class rollbackable_dsu {
 public:
  rollbackable_dsu(int n_)
      : n(n_), fa(n_, -1), sz(n_, 1), id(n_), global_id(n_) {
    for (int i = 0; i < n_; ++i) id[i] = i;
  }
  int find(int i) {
    while (~fa[i]) i = fa[i];
    return i;
  }
  int component_id(int u) { return id[find(u)]; }
  bool merge(int u, int v) {
    ops.push_back({});
    int fu = find(u), fv = find(v);
    if (fu == fv) return false;
    if (sz[fu] < sz[fv]) std::swap(fu, fv);
    auto& version = ops.back();
    version.emplace_back(&sz[fu], sz[fu]);
    version.emplace_back(&fa[fv], fa[fv]);
    version.emplace_back(&id[fu], id[fu]);
    sz[fu] += sz[fv];
    fa[fv] = fu;
    id[fu] = global_id++;
    return true;
  }
  void rollback() {
    for (auto [ptr, val] : ops.back()) {
      *ptr = val;
    }
    ops.pop_back();
  }

 private:
  const int n;
  std::vector<int> fa;
  std::vector<int> sz;

  int global_id;
  std::vector<int> id;
  std::vector<std::vector<std::pair<int*, int>>> ops;
};