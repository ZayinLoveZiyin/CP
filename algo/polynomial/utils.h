namespace polynomial {
template <typename T>
struct utils {
  // calc [x^n]g(x) than f(g(x))=h(x)
  // f[0]=0 should hold
  static T lagrange_inversion(poly<T> f, int n, poly<T> h = poly<T>{0, 1}) {
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
}  // namespace polynomial

using poly_utils = polynomial::utils<zint>;