struct HeavyLightDecomposition {
  HeavyLightDecomposition(int n_)
      : n(n_),
        fa(n_, -1),
        top(n_, -1),
        size(n_, 1),
        dep(n_),
        in(n_),
        out(n_),
        G(n_) {}
  void addEdge(int u, int v) {
    G[u].push_back(v);
    G[v].push_back(u);
  }

  void init(int rt) {
    std::function<void(int)> dfs1 = [&](int u) {
      if (~fa[u]) {
        G[u].erase(std::find(G[u].begin(), G[u].end(), fa[u]));
      }
      for (int v : G[u]) {
        fa[v] = u;
        dep[v] = dep[u] + 1;
        dfs1(v);
        size[u] += size[v];
      }
      std::sort(G[u].begin(), G[u].end(),
                [&](int i, int j) { return size[i] > size[j]; });
    };
    std::function<void(int)> dfs2 = [&](int u) {
      in[u] = seq.size();
      seq.push_back(u);
      for (int v : G[u]) {
        if (v == G[u].front())
          top[v] = top[u];
        else
          top[v] = v;
        dfs2(v);
      }
      out[u] = seq.size() - 1;
    };
    dfs1(rt);
    top[rt] = rt;
    dfs2(rt);
  }

  int lca(int u, int v) {
    while (top[u] != top[v]) {
      if (dep[top[u]] < dep[top[v]]) std::swap(u, v);
      u = fa[top[u]];
    }
    return dep[u] < dep[v] ? u : v;
  }

  // is u ancester of v
  bool isAncester(int u, int v) { return in[u] <= in[v] && out[v] <= out[u]; }

  int dis(int u, int v) { return dep[u] + dep[v] - 2 * dep[lca(u, v)]; }

  int kthAncester(int u, int k) {
    if (dep[u] < k) return -1;
    int d = dep[u] - k;
    while (dep[top[u]] > d) u = fa[top[u]];
    return seq[in[u] + d - dep[u]];
  }

  std::vector<std::pair<int, int>> getPath(int u, int v) {
    std::vector<std::pair<int, int>> lpath, rpath;
    while (top[u] != top[v]) {
      if (dep[top[u]] > dep[top[v]]) {
        lpath.emplace_back(u, top[u]);
        u = fa[top[u]];
      } else {
        rpath.emplace_back(top[v], v);
        v = fa[top[v]];
      }
    }
    std::vector<std::pair<int, int>> path = std::move(lpath);
    path.emplace_back(u, v);
    for (; rpath.size(); rpath.pop_back()) path.push_back(rpath.back());
    return path;
  }

  // root is u, what is father of v
  int rootedFather(int u, int v) {
    if (!isAncester(v, u)) return fa[v];
    return kthAncester(u, dep[u] - dep[v] - 1);
  }

  // root is u, what is kth ancestor of v
  int rootedKthAncester(int u, int v, int k) {
    int lca = this->lca(u, v);
    int dis_u_lca = dep[u] - dep[lca], dis_v_lca = dep[v] - dep[lca];
    if (k > dis_u_lca + dis_v_lca) return -1;
    if (k <= dis_v_lca) return kthAncester(v, k);
    return kthAncester(u, dis_u_lca + dis_v_lca - k);
  }

  int n;
  std::vector<int> fa, top;
  std::vector<int> in, out, seq;
  std::vector<int> size, dep;
  std::vector<std::vector<int>> G;
};