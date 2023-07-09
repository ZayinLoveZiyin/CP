template <typename T,
          typename std::enable_if_t<std::is_arithmetic<T>::value, int> = 0>
struct dijkstra : public weighted_graph<weighted_edge<T>> {
  static constexpr T inf = std::numeric_limits<T>::max();

  std::vector<T> dis, from;
  std::vector<bool> reachable;

  dijkstra(int n)
      : weighted_graph<weighted_edge<T>>(n), dis(n), from(n), reachable(n) {}

  void work(int vs) {
    struct item {
      int u;
      T d;
      bool operator<(const item& I) const { return d > I.d; }
    };
    dis.assign(this->n, inf);
    reachable.assign(this->n, false);
    from.assign(this->n, -1);
    std::priority_queue<item> q;
    q.push({vs, dis[vs] = 0});
    while (q.size()) {
      auto [u, d] = q.top();
      q.pop();
      if (dis[u] < d) continue;
      reachable[u] = true;
      for (int i : this->g[u]) {
        auto [v, w] = this->edges[i];
        if (dis[v] <= dis[u] + w) continue;
        from[v] = u;
        q.push({v, dis[v] = dis[u] + w});
      }
    }
  }
};