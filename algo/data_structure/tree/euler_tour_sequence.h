struct EulerTourSeq {
  EulerTourSeq(int n_)
      : n(n_), fa(n_, -1), size(n_, 1), dep(n_), in(n_), out(n_), G(n_) {}
  void add_edge(int u, int v) {
    G[u].push_back(v);
    G[v].push_back(u);
  }

  void init(int rt) {
    std::function<void(int)> dfs = [&](int u) {
      if (~fa[u]) {
        G[u].erase(std::find(G[u].begin(), G[u].end(), fa[u]));
      }
      in[u] = out[u] = seq.size();
      seq.push_back(u);
      for (int v : G[u]) {
        fa[v] = u;
        dep[v] = dep[u] + 1;
        dfs(v);
        size[u] += size[v];
        out[u] = seq.size();
        seq.push_back(u);
      }
    };
    dfs(rt);
  }

  int n;
  std::vector<int> fa;
  std::vector<int> in, out, seq;
  std::vector<int> size, dep;
  std::vector<std::vector<int>> G;
};
