template <typename T>
struct binomial {
  static T fac(int n) {
    static std::vector<T> f = {1};
    while (f.size() <= n) f.push_back(f.back() * f.size());
    return f[n];
  }
  static T inv(int n) {
    static std::vector<T> inv = {1, 1};
    while (inv.size() <= n)
      inv.push_back(-inv[T::modulus() % inv.size()] *
                    (T::modulus() / inv.size()));
    return inv[n];
  }
  static T finv(int n) {
    static std::vector<T> finv = {1};
    while (finv.size() <= n) finv.push_back(finv.back() * inv(finv.size()));
    return finv[n];
  }
  static T C(int n, int m) {
    if (n < 0 || m < 0 || m > n) return T(0);
    return fac(n) * finv(m) * finv(n - m);
  }
  static T C_bf(int n, int m) {
    if (n < 0 || m < 0 || m > n) return T(0);
    T p = 1;
    for (int i = 0; i < m; ++i) p *= n - i;
    return p * finv(m);
  }
  static T A(int n, int m) {
    if (n < 0 || m < 0 || m > n) return T(0);
    return fac(n) * finv(n - m);
  }
  static T partition_number(int n) {
    static std::vector<int> pentagons;
    auto add_pentagons = [&](long long x) {
      if (x <= std::numeric_limits<int>::max()) pentagons.push_back(x);
    };
    static std::vector<T> pn = {1};
    while (pn.size() <= n) {
      add_pentagons(pn.size() * (3 * pn.size() - 1LL) / 2);
      add_pentagons(pn.size() * (3 * pn.size() + 1LL) / 2);
      T res = 0;
      for (int j = 0; j < pentagons.size() && pentagons[j] <= pn.size(); ++j) {
        if (j & 2)
          res -= pn[pn.size() - pentagons[j]];
        else
          res += pn[pn.size() - pentagons[j]];
      }
      pn.push_back(res);
    }
    return pn[n];
  }
};

using mbinomial = binomial<mint>;