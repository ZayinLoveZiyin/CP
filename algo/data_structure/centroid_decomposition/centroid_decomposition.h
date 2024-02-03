template <class Info>
struct CentroidDecomposition {
  CentroidDecomposition(int n) : n(n), G(n) {}

  void addEdge(int u, int v, Info w = Info()) {
    G[u].emplace_back(v, w);
    G[v].emplace_back(u, w);
  }

  // mainly need to implement centroid_inclusion / u_exclusion
  // i.e. given a bfs sequence, where node i's parent is given as fa[i]
  long long solve(int m) {
    std::vector<bool> vis(n);
    std::vector<int> fa(n, -1);
    std::vector<size_t> size(n);
    std::vector<size_t> max_size(n);

    std::vector<int> dep(n);

    long long ans = 0;
    auto calc = [&](const std::vector<int>& q, int base_len) {
      dep[q.front()] = base_len;
      std::vector<long long> cnt(q.size() + base_len);
      for (int u : q) {
        if (~fa[u]) dep[u] = dep[fa[u]] + 1;
        ++cnt[dep[u]];
      }
    };
    auto centroid_inclusion = [&](const std::vector<int>& q) {
      ans += calc(q, 0);
    };

    auto u_exclusion = [&](const std::vector<int>& q) { ans -= calc(q, 1); };

    auto bfs = [&](int u) {
      std::vector<int> q;
      fa[u] = -1;
      q.push_back(u);
      for (int i = 0; i < q.size(); ++i) {
        int u = q[i];
        for (auto& [v, _] : G[u]) {
          if (vis[v] || fa[u] == v) continue;
          fa[v] = u;
          q.push_back(v);
        }
      }
      return q;
    };
    auto get_centroid = [&](const std::vector<int>& q) {
      int centroid = -1;
      for (int u : q) {
        size[u] = 1;
        max_size[u] = 0;
      }
      for (int i = q.size() - 1; i >= 0; --i) {
        int u = q[i];
        max_size[u] = std::max(max_size[u], q.size() - size[u]);
        if (!~centroid || max_size[u] < max_size[centroid]) centroid = u;
        if (~fa[u]) {
          size[fa[u]] += size[u];
          max_size[fa[u]] = std::max(max_size[fa[u]], size[u]);
        }
      }
      return centroid;
    };
    std::function<void(int, int)> decomposition = [&](int u,
                                                      int last_centroid) {
      auto uq = bfs(u);
      if (~last_centroid) u_exclusion(uq);
      int centroid = get_centroid(uq);
      auto cq = bfs(centroid);
      centroid_inclusion(cq);
      vis[centroid] = true;
      for (auto& [v, _] : G[centroid]) {
        if (vis[v]) continue;
        decomposition(v, centroid);
      }
    };
    decomposition(0, -1);
    return ans;
  }

  int n;
  std::vector<std::vector<std::pair<int, Info>>> G;
};
