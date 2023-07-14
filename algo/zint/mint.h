namespace modular {

template <long long P>
class mint {
 public:
  using type = long long;
  constexpr static type modulus() { return P; }

  constexpr mint() : value(0) {}
  template <typename U,
            typename std::enable_if_t<std::is_integral<U>::value, int> = 0>
  mint(const U& x) {
    value = normalize(x);
  }

  const type& operator()() const { return value; }
  template <typename U,
            typename std::enable_if_t<std::is_integral<U>::value, int> = 0>
  explicit operator U() const {
    return (U)value;
  }

  mint operator-() const { return mint(-value); }

  mint& operator+=(const mint& z) {
    value += z.value;
    if (value >= modulus()) value -= modulus();
    return *this;
  }
  mint& operator-=(const mint& z) {
    value -= z.value;
    if (value < 0) value += modulus();
    return *this;
  }
  mint& operator*=(const mint& z) {
    long long t =
        value * z.value - (long long)(1.0L * value * z.value / P + 0.5L) * P;
    value = normalize(t % P);
    return *this;
  }
  mint& operator/=(const mint& z) { return *this *= z.inv(); }
  mint inv() const { return pow(P - 2); }
  mint pow(long long k) const {
    mint res = 1;
    for (mint z = *this; k; k >>= 1) {
      if (k & 1) res *= z;
      z *= z;
    }
    return res;
  }
  mint operator+(const mint& z) const { return mint(*this) += z; }
  mint operator-(const mint& z) const { return mint(*this) -= z; }
  mint operator*(const mint& z) const { return mint(*this) *= z; }
  mint operator/(const mint& z) const { return mint(*this) /= z; }

  bool operator==(const mint& z) const { return value == z.value; }
  bool operator!=(const mint& z) const { return value != z.value; }
  bool operator<(const mint& z) const { return value < z.value; }
  bool operator>(const mint& z) const { return value > z.value; }

  std::string to_string() const { return std::to_string(value); }

  friend std::ostream& operator<<(std::ostream& os, mint z) {
    return os << z.to_string();
  }

  friend std::istream& operator>>(std::istream& is, mint& z) {
    int64_t t;
    is >> t;
    z = mint(t);
    return is;
  };

 private:
  type value;
  template <typename U>
  static typename std::enable_if_t<std::is_integral<U>::value, type> normalize(
      const U& x) {
    type value;
    if (-modulus() <= x && x < modulus())
      value = (type)x;
    else
      value = (type)(x % modulus());
    if (value < 0) value += modulus();
    return value;
  }
};

}  // namespace modular

using mint = modular::mint<1000000007>;