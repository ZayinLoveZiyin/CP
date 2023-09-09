template <typename T>
class RMQ {
 public:
  RMQ(std::vector<T> a) {
    ST.push_back(a);
    for (int j = 1; j <= std::__lg(a.size()); ++j) {
      ST.push_back(std::vector<T>(a.size() - (1 << j) + 1));
      for (int i = 0; i < ST.back().size(); ++i) {
        ST[j][i] = std::min(ST[j - 1][i], ST[j - 1][i + (1 << (j - 1))]);
      }
    }
    dbg(ST);
  }
  T query_min(int l, int r) {
    int t = std::__lg(r - l + 1);
    return std::min(ST[t][l], ST[t][r - (1 << t) + 1]);
  }

 private:
  std::vector<std::vector<T>> ST;
};