namespace modular {

struct modulus_type {
  static constexpr int primitive_root = 3;
  static constexpr int value = 998244353;
};

template <typename T>
class zint {
 public:
  using type = typename std::decay<decltype(T::value)>::type;

  constexpr static type modulus() { return T::value; }
  constexpr static zint primitive_root() { return zint(T::primitive_root); }

  constexpr zint() : value(0) {}
  template <typename U,
            typename std::enable_if_t<std::is_integral<U>::value, int> = 0>
  zint(const U& x) {
    value = normalize(x);
  }

  const type& operator()() const { return value; }
  template <typename U,
            typename std::enable_if_t<std::is_integral<U>::value, int> = 0>
  explicit operator U() const {
    return (U)value;
  }

  zint operator-() const { return zint(-value); }

  zint& operator+=(const zint& z) {
    value += z.value;
    if (value >= modulus()) value -= modulus();
    return *this;
  }
  zint& operator-=(const zint& z) {
    value -= z.value;
    if (value < 0) value += modulus();
    return *this;
  }
  zint& operator*=(const zint& z) {
    value = normalize((int64_t)value * (int64_t)z.value);
    return *this;
  }
  zint& operator/=(const zint& z) { return *this *= z.inv(); }
  zint pow(long long k) const {
    zint res = 1;
    for (zint z = *this; k; k >>= 1) {
      if (k & 1) res *= z;
      z *= z;
    }
    return res;
  }
  zint inv() const {
    assert(value != 0);
    type a = value, m = modulus();
    type u = 0, v = 1;
    while (a) {
      type t = m / a;
      m -= t * a;
      std::swap(a, m);
      u -= t * v;
      std::swap(u, v);
    }
    assert(m == 1);
    return zint(u);
  }
  zint operator+(const zint& z) const { return zint(*this) += z; }
  zint operator-(const zint& z) const { return zint(*this) -= z; }
  zint operator*(const zint& z) const { return zint(*this) *= z; }
  zint operator/(const zint& z) const { return zint(*this) /= z; }

  static zint random() {
    static std::mt19937_64 engine(
        std::chrono::steady_clock::now().time_since_epoch().count());
    static std::uniform_int_distribution<type> rng(0, modulus() - 1);
    return rng(engine);
  }

  // TODO: support composite number
  bool is_sqrtable() const {
    if (!value) return true;
    return pow((modulus() - 1) / 2) == 1;
  }

  zint sqrt() const {
    assert(is_sqrtable());
    if (modulus() == 2 || value == 0) return *this;
    if (modulus() % 4 == 3) return pow((modulus() + 1) / 4);
    // get w=a^2-*this that is not sqrtable
    zint a, w;
    while (1) {
      a = random();
      w = a * a - *this;
      if (!w.is_sqrtable()) break;
    }
    // x+y*sqrt(w)
    struct complex {
      zint x, y;
    };
    auto multiply_complex = [i2 = w](complex a, complex b) {
      return complex{a.x * b.x + a.y * b.y * i2, a.x * b.y + a.y * b.x};
    };
    complex res = {1, 0}, t = {a, 1};
    for (int k = (modulus() + 1) / 2; k; k >>= 1) {
      if (k & 1) res = multiply_complex(res, t);
      t = multiply_complex(t, t);
    }
    assert(res.x * res.x == *this);
    return std::min(res.x, -res.x);
  }

  bool operator==(const zint& z) const { return value == z.value; }
  bool operator!=(const zint& z) const { return value != z.value; }
  bool operator<(const zint& z) const { return value < z.value; }
  bool operator>(const zint& z) const { return value > z.value; }

  std::string to_string() const { return std::to_string(value); }

  friend std::ostream& operator<<(std::ostream& os, zint z) {
    return os << z.to_string();
  }

  friend std::istream& operator>>(std::istream& is, zint& z) {
    int64_t t;
    is >> t;
    z = zint(t);
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

using zint = modular::zint<modular::modulus_type>;