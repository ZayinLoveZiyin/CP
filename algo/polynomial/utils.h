namespace polynomial {
template <typename T>
struct utils {
  // calc [x^n]h(g(x)) that f(g(x))=x
  // f[0]=0 should hold
  static T lagrange_inversion(poly<T> f, int n, poly<T> h = poly<T>{0, 1}) {
    // format 1: [x^n]h(g(x))=1/n[x^n-1]h'(x)(x/f(x))^n (used)
    // format 2: [x^n]h(g(x))=[x^n]h(x)f'(x)(x/f(x))^(n+1)
    assert(f[0] == 0);
    return (f.divxk(1).pow(n).inv() * h.deriv())[n - 1] * binomial<T>::inv(n);
  }

  // [1/(1-x)]^k % x^n
  static poly<T> const_inverse_poly_pow(int k, int n) {
    poly<T> res(n);
    for (int i = 0; i < n; ++i) res[i] = binomial<T>::C(k + i - 1, k - 1);
    return res;
  }
};

// possible modulus: 469762049, 998244353
template <typename zint1, typename zint2>
struct mtt {
  static std::vector<long long> conv(const std::vector<long long>& a,
                                     const std::vector<long long>& b) {
    auto c1 = internal_conv<zint1>(a, b);
    auto c2 = internal_conv<zint2>(a, b);
    std::vector<long long> res(a.size() + b.size() - 1);
    long long inv_1_of_2 = zint2(zint1::modulus()).inv();
    long long inv_2_of_1 = zint1(zint2::modulus()).inv();
    long long modulus = (long long)zint1::modulus() * zint2::modulus();
    for (int i = 0; i < a.size() + b.size() - 1; ++i) {
      res[i] = c1[i] * inv_2_of_1 % zint1::modulus() * zint2::modulus() +
               c2[i] * inv_1_of_2 % zint2::modulus() * zint1::modulus();
      res[i] %= modulus;
    }
    return res;
  }

 private:
  template <typename zint>
  static std::vector<long long> internal_conv(const std::vector<long long>& a,
                                              const std::vector<long long>& b) {
    polynomial::poly<zint> A(a.size()), B(b.size());
    for (int i = 0; i < a.size(); ++i) A[i] = a[i];
    for (int i = 0; i < b.size(); ++i) B[i] = b[i];
    polynomial::poly<zint> C = A * B;
    std::vector<long long> res(a.size() + b.size() - 1);
    for (int i = 0; i < C.size(); ++i) res[i] = C[i]();
    return res;
  }
};

}  // namespace polynomial

using poly_utils = polynomial::utils<zint>;