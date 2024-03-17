// 2*i: false, 2*i+1: true
struct TwoSAT : public DirectionalTarjan {
  TwoSAT(int n) : n(n), DirectionalTarjan(2 * n) {}

  inline int T(int i) { return i << 1 | 1; }
  inline int F(int i) { return i << 1; }
  inline int node_id(int i, int v) {
    assert(v == 0 || v == 1);
    return i << 1 | v;
  }
  inline int var_id(int u) { return u >> 1; }
  inline int opposite(int u) { return u ^ 1; }
  void addOr(int u, int v) {
    DirectionalTarjan::addEdge(opposite(u), v);
    DirectionalTarjan::addEdge(opposite(v), u);
  }
  // do not call addEdge for mistake!
  void addEdge(int u, int v) { assert(0); }

  // find one possible solution, O(n)
  std::optional<std::vector<bool>> getOneSolution() {
    auto [bcnt, bel] = getSCC();
    std::vector<bool> res(n);
    for (int i = 0; i < n; ++i) {
      if (bel[T(i)] == bel[F(i)]) return std::nullopt;
      res[i] = bel[T(i)] < bel[F(i)];
    }
    return res;
  }

  // find at most m solutions, O(mn^2/w)
  template <int LEN = 1 << 20>
  std::vector<std::vector<bool>> getMultipleSolution(int m) {
    if (LEN > 1 && 2 * n <= LEN / 2) return getMultipleSolution<LEN / 2>(m);
    assert(2 * n <= LEN);
    auto [bcnt, bel] = getSCC();
    for (int i = 0; i < n; ++i) {
      if (bel[T(i)] == bel[F(i)]) return {};
    }
    std::vector<int> order(2 * n);
    std::iota(order.begin(), order.end(), 0);
    std::sort(order.begin(), order.end(),
              [&](int u, int v) { return bel[u] < bel[v]; });
    std::vector<std::bitset<LEN>> reachable(2 * n);
    std::vector<std::bitset<LEN>> reachable_opposite(2 * n);

    for (int u : order) {
      for (int v : G[u]) reachable[u] |= reachable[v];
      for (int v = 0; v < n * 2; ++v)
        if (bel[u] == bel[v]) reachable[u].set(v);
      for (int v = 0; v < n * 2; ++v) {
        if (reachable[u][v]) reachable_opposite[u].set(opposite(v));
      }
    }

    std::vector<std::vector<bool>> res;
    std::bitset<LEN> vised;
    std::function<bool(int)> dfs = [&](int i) {
      if (i == n) {
        res.emplace_back(n);
        for (int i = 0; i < n; ++i) res.back()[i] = vised[T(i)];
        return res.size() < m;
      }

      if (vised[T(i)] || vised[F(i)]) {
        if (!dfs(i + 1)) return false;
      } else {
        auto test = [&](int u) {
          if (!reachable_opposite[u][u] &&
              (reachable_opposite[u] & vised).none()) {
            std::bitset<LEN> backup = vised;
            vised |= reachable[u];
            if (!dfs(i + 1)) return false;
            vised = backup;
          }
          return true;
        };
        if (!test(T(i))) return false;
        if (!test(F(i))) return false;
      }
      return true;
    };
    dfs(0);
    assert(!m || res.size());
    return res;
  }

 protected:
  int n;
};