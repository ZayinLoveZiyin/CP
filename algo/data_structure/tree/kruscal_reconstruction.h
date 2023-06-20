heavy_light_decomposition tree_small(n);
disjoint_set_union dsu_small(n);
for (int i = n - 1; i >= 0; --i)
  for (int j : G[i]) {
    if (j < i) continue;
    tree_small.add_edge(i, dsu_small.find(j));
    dsu_small.merge(i, j);
  }
tree_small.init(0);

heavy_light_decomposition tree_big(n);
disjoint_set_union dsu_big(n);
for (int i = 0; i < n; ++i)
  for (int j : G[i]) {
    if (j > i) continue;
    tree_big.add_edge(i, dsu_big.find(j));
    dsu_big.merge(i, j);
  }
tree_big.init(n - 1);