struct TopoSort {
  static std::optional<std::vector<int>> solve(
      const std::vector<std::vector<int>>& G) {
    std::vector<int> deg(G.size());
    for (int i = 0; i < G.size(); ++i)
      for (int j : G[i]) ++deg[j];
    std::queue<int> q;
    for (int i = 0; i < G.size(); ++i)
      if (!deg[i]) q.push(i);
    std::vector<int> seq;
    while (q.size()) {
      int i = q.front();
      q.pop();
      seq.push_back(i);
      for (int j : G[i]) {
        --deg[j];
        if (!deg[j]) q.push(j);
      }
    }
    if (seq.size() != G.size()) return std::nullopt;
    return seq;
  }
};