namespace matrix {

struct size3 {
  static constexpr int size = 3;
};

template <typename S, typename T>
struct matrix : public std::vector<std::vector<T>> {
  using std::vector<std::vector<T>>::vector;

  matrix(int x = 0) {
    this->resize(size());
    for (int i = 0; i < size(); ++i) {
      this->at(i).resize(size());
      for (int j = 0; j < size(); ++j) this->at(i)[j] = 0;
    }
    for (int i = 0; i < size(); ++i) this->at(i)[i] = x;
  }

  constexpr static int size() { return S::size; }

  friend matrix operator*(const matrix& lhs, const matrix& rhs) {
    matrix res(0);
    for (int i = 0; i < size(); ++i)
      for (int k = 0; k < size(); ++k)
        for (int j = 0; j < size(); ++j) res[i][j] += lhs[i][k] * rhs[k][j];
    return res;
  }

  matrix pow(long long k) {
    matrix res(1);
    for (matrix a = *this; k; k >>= 1) {
      if (k & 1) res = res * a;
      a = a * a;
    }
    return res;
  }

  static matrix merge(const matrix& lhs, const matrix& rhs) {
    return lhs * rhs;
  }

  static matrix construct() {
    // TODO
  }
};
}  // namespace matrix

using matrix3 = matrix::matrix<matrix::size3, zint>;