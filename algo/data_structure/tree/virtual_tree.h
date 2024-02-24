struct VirtualTree {
  HeavyLightDecomposition* hld;
  std::vector<bool> key_vert;
  std::vector<int> involved_vert;
  std::vector<std::vector<int>> T;
  VirtualTree(HeavyLightDecomposition* hld)
      : hld(hld), key_vert(hld->n), involved_vert(hld->n), T(hld->n) {}

  void build(std::vector<int> vert) {
    std::sort(vert.begin(), vert.end(),
              [&](int u, int v) { return hld->in[u] < hld->in[v]; });
    std::vector<int> stk = {0};
    auto pop_stk = [&]() {
      int v = stk.back();
      stk.pop_back();
      involved_vert.push_back(v);
      if (stk.size()) T[stk.back()].push_back(v);
    };
    for (int u : vert) {
      key_vert[u] = true;
      int p = hld->lca(u, stk.back());
      if (stk.back() != p) {
        while (stk.size() && hld->dep[stk.back()] > hld->dep[p]) pop_stk();
        assert(stk.size());
        T[p].push_back(T[stk.back()].back());
        T[stk.back()].pop_back();
      }
      if (stk.back() != p) stk.push_back(p);
      if (stk.back() != u) stk.push_back(u);
    }
    while (stk.size()) pop_stk();
  }

  // remeber to clear the info of involved verts
  void clear() {
    for (int u : involved_vert) {
      T[u].clear();
      key_vert[u] = false;
    }
    involved_vert.clear();
  }
};