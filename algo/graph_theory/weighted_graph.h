template <typename T>
struct weighted_edge {
  int to;
  T weight;
  weighted_edge(int to, T weight) : to(to), weight(weight) {}
};

template <typename T>
struct weighted_graph {
  const int n;
  std::vector<T> edges;
  std::vector<std::vector<int>> g;
  weighted_graph(int n) : n(n), g(n) {}

  void add_edge(int u, T edge) {
    assert(0 <= u && u < n);
    g[u].push_back(edges.size());
    edges.push_back(edge);
  }
};