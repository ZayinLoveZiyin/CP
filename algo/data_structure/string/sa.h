struct SuffixArray {
  int n;
  std::string s;
  // lc[0]=0 is meaningless
  std::vector<int> sa, rk, lc;
  SuffixArray(const std::string& s)
      : s(s), n(s.size()), sa(s.size()), rk(s.size()), lc(s.size()) {
    std::iota(sa.begin(), sa.end(), 0);
    std::sort(sa.begin(), sa.end(), [&](int a, int b) { return s[a] < s[b]; });
    rk[sa[0]] = 0;
    for (int i = 1; i < n; ++i)
      rk[sa[i]] = rk[sa[i - 1]] + (s[sa[i]] != s[sa[i - 1]]);
    std::vector<int> tmp, cnt(n);
    tmp.reserve(n);
    for (int k = 1; rk[sa[n - 1]] < n - 1; k *= 2) {
      tmp.clear();
      for (int i = 0; i < k; ++i) tmp.push_back(n - k + i);
      for (auto i : sa)
        if (i >= k) tmp.push_back(i - k);
      cnt.assign(n, 0);
      for (int i = 0; i < n; ++i) ++cnt[rk[i]];
      for (int i = 1; i < n; ++i) cnt[i] += cnt[i - 1];
      for (int i = n - 1; i >= 0; --i) sa[--cnt[rk[tmp[i]]]] = tmp[i];
      std::swap(rk, tmp);
      rk[sa[0]] = 0;
      for (int i = 1; i < n; ++i) {
        rk[sa[i]] = rk[sa[i - 1]];
        if (tmp[sa[i - 1]] < tmp[sa[i]] || sa[i - 1] + k == n ||
            tmp[sa[i - 1] + k] < tmp[sa[i] + k])
          ++rk[sa[i]];
      }
    }
    for (int i = 0, j = 0; i < n; ++i) {
      if (!rk[i]) {
        j = 0;
      } else {
        if (j) --j;
        int k = sa[rk[i] - 1];
        while (i + j < n && k + j < n && s[i + j] == s[k + j]) ++j;
        lc[rk[i]] = j;
      }
    }
  }
};

class LongestCommonPrefix {
 public:
  LongestCommonPrefix(SuffixArray* sa) : sa(sa), st(sa->lc) {}

  int lcp(int i, int j) {
    if (i == j) return sa->n - i;
    int l = sa->rk[i], r = sa->rk[j];
    if (l > r) std::swap(l, r);
    return st.queryMin(l + 1, r);
  }

 private:
  SuffixArray* sa;
  SparseTable<int> st;
};