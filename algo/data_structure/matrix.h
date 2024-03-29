namespace matrix {

template <typename T>
struct Matrix : public std::vector<std::vector<T>> {
  int n, m;
  using std::vector<std::vector<T>>::vector;

  Matrix(int n, int m) : n(n), m(m) { this->resize(n, std::vector<T>(m)); }

  friend Matrix operator*(const Matrix& lhs, const Matrix& rhs) {
    assert(lhs.m == rhs.n);
    Matrix res(lhs.n, rhs.m);
    for (int i = 0; i < lhs.n; ++i)
      for (int k = 0; k < lhs.m; ++k)
        for (int j = 0; j < rhs.m; ++j) res[i][j] += lhs[i][k] * rhs[k][j];
    return res;
  }

  Matrix pow(long long k) const {
    assert(n == m);
    Matrix res(n, m);
    for (int i = 0; i < n; ++i) res[i][i] = 1;
    for (Matrix a = *this; k; k >>= 1) {
      if (k & 1) res = res * a;
      a = a * a;
    }
    return res;
  }

  T determinant() const {
    assert(n == m);
    std::vector<std::vector<T>> v = *this;
    T res = 1;
    for (int i = 0; i < n; ++i) {
      int pivot = i;
      for (int k = 0; k < n; ++k)
        if (v[k][i]) pivot = i;
      if (i != pivot) {
        std::swap(v[i], v[pivot]);
        res = -res;
      }
      if (!v[i][i]) return T(0);
      for (int k = i + 1; k < n; ++k) {
        T t = v[k][i] / v[i][i];
        for (int j = 0; j < n; ++j) v[k][j] -= v[i][j] * t;
      }
      dbg(v);
    }
    for (int i = 0; i < n; ++i) res *= v[i][i];
    return res;
  }

  T sum() const {
    T res = 0;
    for (int i = 0; i < n; ++i) res += rowsum(i);
    return res;
  }
  T rowsum(int i) const {
    T res = 0;
    for (int j = 0; j < m; ++j) res += this->at(i)[j];
    return res;
  }
  T colsum(int j) const {
    T res = 0;
    for (int i = 0; i < n; ++i) res += this->at(i)[j];
    return res;
  }

  static Matrix merge(const Matrix& lhs, const Matrix& rhs) {
    return lhs * rhs;
  }

  static Matrix construct() {
    // TODO
  }
};
}  // namespace matrix

using Matrix = matrix::Matrix<double>;