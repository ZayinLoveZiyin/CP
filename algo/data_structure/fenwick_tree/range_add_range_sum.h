
// 0-index based, support
// 1. range ai+=d*i^j [0<=j<=AK]
// 2. maintaining ai*i^j [0<=j<=QK]
template <typename T, int AK = 0, int QK = 0>
class FenwickTreeRangeAddRangeSum {
 public:
  FenwickTreeRangeAddRangeSum(int n_)
      : n(n_),
        delta(AK + 1, std::vector<T>(n)),
        tree(AK + 1, std::vector(QK + 1, std::vector<T>(n))),
        power_sum(AK + QK + 1, std::vector<T>(n + 1)) {
    std::vector<T> power(n + 1, 1);
    for (int k = 0; k <= AK + QK; ++k) {
      T sum = 0;
      dbg(k, AK, QK);
      for (int i = 0; i <= n; ++i) {
        power_sum[k][i] = sum;
        sum += power[i];
        power[i] *= i;
      }
    }
  }

  // a[i]+=v*i^d, i>=x
  // O(klogn)
  void suffixAdd(int x, T v, int ak = 0) {
    assert(0 <= ak && ak <= AK);
    for (int i = x + 1; i <= n; i += i & -i) {
      delta[ak][i - 1] += v;
      for (int qk = 0; qk <= QK; ++qk) {
        tree[ak][qk][i - 1] += v * power_sum[ak + qk][x];
      }
    }
  }

  void suffixAdd(int x, std::vector<T> cof) {
    assert(cof.size() <= AK + 1);
    for (int i = x + 1; i <= n; i += i & -i) {
      for (int ak = 0; ak < cof.size(); ++ak) {
        delta[ak][i - 1] += cof[ak];
        for (int qk = 0; qk <= QK; ++qk) {
          tree[ak][qk][i - 1] += cof[ak] * power_sum[ak + qk][x];
        }
      }
    }
  }

  // \sum[i=0...x] ai^k
  // O(k^2logn)
  std::vector<T> prefixSum(int x) {
    std::vector<T> res(QK + 1);
    for (int qk = 0; qk <= QK; ++qk) {
      for (int ak = 0; ak <= AK; ++ak) {
        auto L = T(), R = T();
        for (int i = x + 1; i > 0; i -= i & -i) {
          L += delta[ak][i - 1];
          R += tree[ak][qk][i - 1];
        }
        res[qk] += L * power_sum[ak + qk][x + 1] - R;
      }
    }
    return res;
  }

  T prefixSum(int x, int qk) {
    assert(0 <= qk && qk <= QK);
    auto res = T();
    for (int ak = 0; ak <= AK; ++ak) {
      auto L = T(), R = T();
      for (int i = x + 1; i > 0; i -= i & -i) {
        L += delta[ak][i - 1];
        R += tree[ak][qk][i - 1];
      }
      res += L * power_sum[ak + qk][x + 1] - R;
    }
    return res;
  }

  void rangeAdd(int l, int r, std::vector<T> cof) {
    suffixAdd(l, cof);
    for (T& x : cof) x = -x;
    suffixAdd(r + 1, cof);
  }

  void rangeAdd(int l, int r, T v, int ak = 0) {
    suffixAdd(l, v, ak);
    suffixAdd(r + 1, -v, ak);
  }
  T rangeSum(int l, int r, int qk = 0) {
    return prefixSum(r, qk) - prefixSum(l - 1, qk);
  }

 private:
  int n;
  std::vector<std::vector<T>> delta;
  std::vector<std::vector<std::vector<T>>> tree;
  std::vector<std::vector<T>> power_sum;
};