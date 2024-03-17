struct DirectionalTarjan {
  DirectionalTarjan(int n_) : n(n_), dfn(n_), low(n_), G(n_) {}

  void addEdge(int u, int v) { G[u].push_back(v); }

  std::pair<int, std::vector<int>> getSCC() {
    dfn.assign(n, -1);
    low.assign(n, -1);
    int timestamp = 0, bel_id = 0;
    std::stack<int> stk;
    std::vector<int> bel(n, -1);
    std::function<void(int)> dfs = [&](int u) {
      dfn[u] = low[u] = timestamp++;
      stk.push(u);

      for (int v : G[u]) {
        if (!~dfn[v]) {
          dfs(v);
          low[u] = std::min(low[u], low[v]);
        } else if (!~bel[v]) {
          low[u] = std::min(low[u], dfn[v]);
        }
      }

      if (dfn[u] == low[u]) {
        while (1) {
          int v = stk.top();
          bel[v] = bel_id;
          stk.pop();
          if (u == v) break;
        }
        ++bel_id;
      }
    };
    for (int i = 0; i < n; ++i)
      if (!~dfn[i]) {
        dfs(i);
      }
    return {bel_id, bel};
  }

  int n;
  std::vector<int> dfn, low;
  std::vector<std::vector<int>> G;
};
