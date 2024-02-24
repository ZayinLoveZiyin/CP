template <typename T>
class SparseTable {
 public:
  SparseTable(std::vector<T> a) {
    table.push_back(a);
    for (int j = 1; j <= std::__lg(a.size()); ++j) {
      table.emplace_back(a.size() - (1 << j) + 1);
      for (int i = 0; i < table.back().size(); ++i) {
        table[j][i] =
            std::min(table[j - 1][i], table[j - 1][i + (1 << (j - 1))]);
      }
    }
  }
  T queryMin(int l, int r) {
    assert(l <= r);
    int t = std::__lg(r - l + 1);
    return std::min(table[t][l], table[t][r - (1 << t) + 1]);
  }

 private:
  std::vector<std::vector<T>> table;
};