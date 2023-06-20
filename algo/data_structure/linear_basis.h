template <typename T, int width = sizeof(T) * 8>
class linear_basis {
 public:
  static_assert(std::is_integral<T>::value);
  linear_basis() { bases_.resize(width); }
  size_t basic_count() { return basic_count_; }
  std::vector<T> bases() { return bases_; }
  bool insert(T x) {
    for (int i = width - 1; i >= 0; --i) {
      if (~x >> i & 1) continue;
      if (bases_[i]) {
        x ^= bases_[i];
      } else {
        ++basic_count_;
        bases_[i] = x;
        return true;
      }
    }
    return false;
  }

  T maximum() const {
    T res = 0;
    for (int i = width - 1; i >= 0; --i) res = std::max(res, res ^ bases_[i]);
    return res;
  }

  friend linear_basis operator+(linear_basis lhs, linear_basis rhs) {
    for (T x : lhs.bases()) rhs.insert(x);
    return rhs;
  }

 private:
  size_t basic_count_ = 0;
  std::vector<T> bases_;
};
