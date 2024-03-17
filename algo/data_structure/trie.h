template <int BitWidth = 30>
struct Trie {
  Trie() { newnode(); }
  void insert(int x) {
    int k = 0;
    nums[k].insert(x);
    for (int b = BitWidth - 1; b >= 0; --b) {
      int c = x >> b & 1;
      if (!~son[k][c]) son[k][c] = newnode();
      k = son[k][c];
      nums[k].insert(x);
    }
  }
  int newnode() {
    int k = son.size();
    son.emplace_back(2, -1);
    nums.emplace_back();
    return k;
  }
  std::vector<std::vector<int>> son;
  std::vector<std::set<int>> nums;
};
