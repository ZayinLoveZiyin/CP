struct SegmentTree {
  SegmentTree(int n) : n(n) {
    std::function<int(int, int)> build = [&](int l, int r) {
      int k = size();
      ls.emplace_back(-1);
      rs.emplace_back(-1);
      if (l == r) return k;
      int mid = (l + r) >> 1;
      ls[k] = build(l, mid);
      rs[k] = build(mid + 1, r);
      return k;
    };
    build(0, n - 1);
  };

  int size() { return ls.size(); }

  int point(int i) {
    std::vector<int> res = range(i, i);
    assert(res.size() == 1);
    return res[0];
  }

  std::vector<int> range(int a, int b) {
    std::vector<int> res;
    std::function<void(int, int, int)> find = [&](int k, int l, int r) {
      if (a > r || l > b) return;
      if (a <= l && r <= b) {
        res.push_back(k);
        return;
      }
      int mid = (l + r) >> 1;
      find(ls[k], l, mid);
      find(rs[k], mid + 1, r);
    };
    find(0, 0, n - 1);
    return res;
  }

  int n;
  std::vector<int> ls, rs;
};