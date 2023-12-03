namespace modular {

template <std::uint32_t mod>
class MontgomeryModInt {
 public:
  using i32 = std::int32_t;
  using u32 = std::uint32_t;
  using u64 = std::uint64_t;
  using m32 = MontgomeryModInt;

  using value_type = u32;

  static constexpr u32 modulus() { return mod; }

  static constexpr u32 primitive_root() { return pr; }

  constexpr MontgomeryModInt() : MontgomeryModInt(0) {}

  template <typename T, std::enable_if_t<std::is_integral_v<T>, int> = 0>
  constexpr MontgomeryModInt(T v)
      : v_(reduce(u64(v % i32(mod) + i32(mod)) * r2)) {}

  constexpr MontgomeryModInt(const m32&) = default;

  constexpr u32 get() const { return norm(reduce(v_)); }

  template <typename T, std::enable_if_t<std::is_integral_v<T>, int> = 0>
  explicit constexpr operator T() const {
    return T(get());
  }

  constexpr m32 operator-() const {
    m32 res;
    res.v_ = (mod2 & -(v_ != 0)) - v_;
    return res;
  }

  constexpr m32 inv() const {
    i32 x1 = 1, x3 = 0, a = get(), b = mod;
    while (b != 0) {
      i32 q = a / b, x1_old = x1, a_old = a;
      x1 = x3, x3 = x1_old - x3 * q, a = b, b = a_old - b * q;
    }
    return m32(x1);
  }

  static m32 random() {
    static std::mt19937_64 engine(
        std::chrono::steady_clock::now().time_since_epoch().count());
    static std::uniform_int_distribution<u32> rng(0, modulus() - 1);
    return rng(engine);
  }

  bool is_sqrtable() const {
    if (!get()) return true;
    return pow((modulus() - 1) / 2) == 1;
  }

  constexpr m32 sqrt() const {
    assert(is_sqrtable());
    if (modulus() == 2 || !get()) return *this;
    if (modulus() % 4 == 3) return pow((modulus() + 1) / 4);
    // get w=a^2-*this that is not sqrtable
    m32 a, w;
    while (1) {
      a = random();
      w = a * a - *this;
      if (!w.is_sqrtable()) break;
    }
    // // x+y*sqrt(w)
    struct complex {
      m32 x, y;
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

  constexpr m32& operator=(const m32&) = default;

  constexpr m32& operator+=(const m32& rhs) {
    v_ += rhs.v_ - mod2;
    v_ += mod2 & -(v_ >> 31);
    return *this;
  }
  constexpr m32& operator-=(const m32& rhs) {
    v_ -= rhs.v_;
    v_ += mod2 & -(v_ >> 31);
    return *this;
  }
  constexpr m32& operator*=(const m32& rhs) {
    v_ = reduce(u64(v_) * rhs.v_);
    return *this;
  }
  constexpr m32& operator/=(const m32& rhs) { return operator*=(rhs.inv()); }
  friend constexpr m32 operator+(const m32& lhs, const m32& rhs) {
    return m32(lhs) += rhs;
  }
  friend constexpr m32 operator-(const m32& lhs, const m32& rhs) {
    return m32(lhs) -= rhs;
  }
  friend constexpr m32 operator*(const m32& lhs, const m32& rhs) {
    return m32(lhs) *= rhs;
  }
  friend constexpr m32 operator/(const m32& lhs, const m32& rhs) {
    return m32(lhs) /= rhs;
  }
  friend constexpr bool operator==(const m32& lhs, const m32& rhs) {
    return norm(lhs.v_) == norm(rhs.v_);
  }
  friend constexpr bool operator!=(const m32& lhs, const m32& rhs) {
    return norm(lhs.v_) != norm(rhs.v_);
  }
  friend constexpr bool operator<(const m32& lhs, const m32& rhs) {
    return lhs.get() < rhs.get();
  }

  friend std::istream& operator>>(std::istream& is, m32& rhs) {
    i32 x;
    is >> x;
    rhs = m32(x);
    return is;
  }
  friend std::ostream& operator<<(std::ostream& os, const m32& rhs) {
    return os << rhs.get();
  }

  constexpr m32 pow(u64 y) const {
    m32 res(1), x(*this);
    for (; y != 0; y >>= 1, x *= x)
      if (y & 1) res *= x;
    return res;
  }

 private:
  static constexpr u32 get_r() {
    u32 two = 2, iv = mod * (two - mod * mod);
    iv *= two - mod * iv;
    iv *= two - mod * iv;
    return iv * (mod * iv - two);
  }

  static constexpr u32 get_primitive_root() {
    u32 tmp[32] = {};
    int cnt = 0;
    const u32 phi = mod - 1;
    u32 m = phi;
    for (u32 i = 2; i * i <= m; ++i) {
      if (m % i == 0) {
        tmp[cnt++] = i;
        do {
          m /= i;
        } while (m % i == 0);
      }
    }
    if (m != 1) tmp[cnt++] = m;
    for (m32 res = 2;; res += 1) {
      bool f = true;
      for (int i = 0; i < cnt && f; ++i) f &= res.pow(phi / tmp[i]) != 1;
      if (f) return u32(res);
    }
  }

  static constexpr u32 reduce(u64 x) {
    return (x + u64(u32(x) * r) * mod) >> 32;
  }
  static constexpr u32 norm(u32 x) {
    return x - (mod & -((mod - 1 - x) >> 31));
  }

  u32 v_;

  static constexpr u32 pr = get_primitive_root();
  static constexpr u32 r = get_r();
  static constexpr u32 r2 = -u64(mod) % mod;
  static constexpr u32 mod2 = mod << 1;

  static_assert((mod & 1) == 1, "mod % 2 == 0\n");
  static_assert(-r * mod == 1, "???\n");
  static_assert((mod & (3U << 30)) == 0, "mod >= (1 << 30)\n");
  static_assert(mod != 1, "mod == 1\n");
};

}  // namespace modular

using mint = modular::MontgomeryModInt<998244353>;
