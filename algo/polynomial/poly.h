namespace polynomial {

template <typename T>
class poly : public std::vector<T> {
 public:
  using std::vector<T>::vector;

  poly(std::string s) {
    for (int i = 0; i < s.size();) {
      auto scan_num = [&]() -> long long {
        int sgn = 1;
        if (s[i] == '-') sgn = -1, ++i;
        if (s[i] == '+') sgn = 1, ++i;
        if (i == s.size() || !std::isdigit(s[i])) return sgn;
        long long num = 0;
        while (i < s.size() && std::isdigit(s[i]))
          num = num * 10 + s[i++] - '0';
        return sgn * num;
      };
      auto add_item = [&](size_t exponent, T coeff) {
        if (exponent >= this->size()) this->resize(exponent + 1);
        this->at(exponent) = coeff;
      };
      T coeff = scan_num();
      if (i == s.size() || s[i] != 'x')
        add_item(0, coeff);
      else {
        size_t exponent = 1;
        if (s[++i] == '^') {
          ++i;
          exponent = scan_num();
        }
        add_item(exponent, coeff);
      }
    }
  }

  int deg() const { return this->size() - 1; }
  poly operator-() const {
    poly ans = *this;
    for (auto& x : ans) x = -x;
    return ans;
  }
  T operator()(const T& x) const {
    T ans = 0;
    for (int i = this->size() - 1; i >= 0; --i) ans = ans * x + this->at(i);
    return ans;
  }
  T operator[](int idx) const {
    if (0 <= idx && idx < this->size()) return this->at(idx);
    return 0;
  }
  T& operator[](int idx) {
    if (idx >= this->size()) this->resize(idx + 1);
    return this->at(idx);
  }

  poly rev() const {
    poly res(*this);
    std::reverse(res.begin(), res.end());
    return res;
  }

  poly mulxk(size_t k) const {
    poly res = *this;
    res.insert(res.begin(), k, 0);
    return res;
  }
  poly divxk(size_t k) const {
    if (this->size() <= k) return {};
    return poly(this->begin() + k, this->end());
  }
  poly modxk(size_t k) const {
    k = std::min(k, this->size());
    return poly(this->begin(), this->begin() + k);
  }

  poly& operator*=(poly p);
  poly& operator+=(const poly& p) {
    this->resize(std::max(this->size(), p.size()));
    for (int i = 0; i < this->size(); ++i) this->at(i) += p[i];
    return this->normalize();
  }
  poly& operator-=(const poly& p) {
    this->resize(std::max(this->size(), p.size()));
    for (int i = 0; i < this->size(); ++i) this->at(i) -= p[i];
    return this->normalize();
  }
  poly& operator/=(const poly& p) {
    if (this->size() < p.size()) return *this = {};
    int len = this->size() - p.size() + 1;
    return *this = (this->rev().modxk(len) * p.rev().inv(len))
                       .modxk(len)
                       .rev()
                       .normalize();
  }
  poly& operator%=(const poly& p) {
    return *this = (*this - (*this / p) * p).normalize();
  }
  poly& operator*=(const T& x) {
    for (int i = 0; i < this->size(); ++i) this->at(i) *= x;
    return *this;
  }
  poly& operator/=(const T& x) { return *this *= x.inv(); }
  poly operator*(const poly& p) const { return poly(*this) *= p; }
  poly operator+(const poly& p) const { return poly(*this) += p; }
  poly operator-(const poly& p) const { return poly(*this) -= p; }
  poly operator/(const poly& p) const { return poly(*this) /= p; }
  poly operator%(const poly& p) const { return poly(*this) %= p; }
  poly operator*(const T& x) const { return poly(*this) *= x; }
  poly operator/(const T& x) const { return poly(*this) /= x; }

  // (quotient, remainder)
  std::pair<poly, poly> divmod(const poly& p) const {
    poly d = *this / p;
    return std::make_pair(d, (*this - d * p).normalize());
  }

  poly deriv() const {
    if (this->empty()) return {};
    poly res(this->size() - 1);
    for (int i = 0; i < this->size() - 1; ++i) {
      res[i] = this->at(i + 1) * (i + 1);
    }
    return res;
  }
  poly integr(T c = 0) const {
    poly res(this->size() + 1);
    for (int i = 0; i < this->size(); ++i) {
      res[i + 1] = this->at(i) / (i + 1);
    }
    res[0] = c;
    return res;
  }

  // mod x^k
  poly inv(int k = -1) const {
    if (!~k) k = this->size();
    poly res = {this->front().inv()};
    for (int len = 2; len < k * 2; len <<= 1) {
      res = (res * (poly{2} - this->modxk(len) * res)).modxk(len);
    }
    return res.modxk(k);
  }

  // mod x^k
  poly sqrt(int k = -1) const {
    if (!~k) k = this->size();
    poly res = {this->at(0).sqrt()};
    for (int len = 2; len < k * 2; len <<= 1) {
      res = (res + (this->modxk(len) * res.inv(len)).modxk(len)) / 2;
    }
    return res.modxk(k);
  }

  // mod x^k, a0=1 should hold
  poly log(int k = -1) const {
    assert(this->at(0) == 1);
    if (!~k) k = this->size();
    return (this->deriv() * this->inv(k)).integr().modxk(k);
  }
  // mod x^k, a0=0 should hold
  poly exp(int k = -1) const {
    assert(this->at(0) == 0);
    if (!~k) k = this->size();
    poly res = {1};
    for (int len = 2; len < k * 2; len <<= 1) {
      res = (res * (poly{1} - res.log(len) + this->modxk(len))).modxk(len);
    }
    return res.modxk(k);
  }
  // p^c mod x^k
  poly pow(int c, int k = -1) const {
    if (!~k) k = this->size();
    int i = 0;
    while (i < this->size() && !this->at(i)) ++i;
    if (i == this->size() || 1LL * i * c >= k) return {};
    T ai = this->at(i);
    poly f = this->divxk(i) * ai.inv();
    return (f.log(k - i * c) * c).exp(k - i * c).mulxk(i * c) * ai.pow(c);
  }

  // evaluate and interpolate
  struct product_tree {
    int l, r;
    std::unique_ptr<product_tree> lson = nullptr, rson = nullptr;
    poly product;
    product_tree(int l, int r) : l(l), r(r) {}

    static std::unique_ptr<product_tree> build(
        const std::vector<T>& xs, std::function<poly(T)> get_poly) {
      std::function<std::unique_ptr<product_tree>(int, int)> build =
          [&](int l, int r) {
            auto rt = std::make_unique<product_tree>(l, r);
            if (l == r) {
              rt->product = get_poly(xs[l]);
            } else {
              int mid = (l + r) >> 1;
              rt->lson = build(l, mid);
              rt->rson = build(mid + 1, r);
              rt->product = rt->lson->product * rt->rson->product;
            }
            return rt;
          };
      return build(0, xs.size() - 1);
    }
  };

  poly mulT(poly p) const {
    if (p.empty()) return {};
    return ((*this) * p.rev()).divxk(p.size() - 1);
  }

  std::vector<T> evaluate(std::vector<T> xs) const {
    if (this->empty()) return std::vector<T>(xs.size());
    std::unique_ptr<product_tree> rt = product_tree::build(xs, [&](T x) {
      return poly{1, -x};
    });
    return evaluate_internal(xs, rt);
  }

  static poly interpolate(std::vector<T> xs, std::vector<T> ys) {
    assert(xs.size() == ys.size());
    if (xs.empty()) return {};
    std::unique_ptr<product_tree> rt = product_tree::build(xs, [&](T x) {
      return poly{1, -x};
    });
    std::vector<T> coef = rt->product.rev().deriv().evaluate_internal(xs, rt);
    for (int i = 0; i < ys.size(); ++i) coef[i] = ys[i] * coef[i].inv();
    std::function<poly(product_tree*)> solve = [&](product_tree* rt) {
      if (rt->l == rt->r) {
        return poly{coef[rt->l]};
      } else {
        return solve(rt->lson.get()) * rt->rson->product.rev() +
               solve(rt->rson.get()) * rt->lson->product.rev();
      }
    };
    return solve(rt.get());
  }

  // a0=0 must hold
  poly cos(int k = -1) const {
    assert(this->at(0) == 0);
    if (!~k) k = this->size();
    T i = T::root().pow((T::modulus() - 1) / 4);
    poly x = *this * i;
    return (x.exp(k) + (-x).exp(k)) / 2;
  }

  // a0=0 must hold
  poly sin(int k = -1) const {
    assert(this->at(0) == 0);
    if (!~k) k = this->size();
    T i = T::root().pow((T::modulus() - 1) / 4);
    poly x = *this * i;
    return (x.exp(k) - (-x).exp(k)) / (i * 2);
  }

  // a0=0 must hold
  poly tan(int k = -1) const { return this->sin(k) / this->cos(k); }

  poly acos(int k = -1) const {
    const poly& x = *this;
    return (-x.deriv() * (poly{1} - x * x).sqrt().inv()).integr();
  };
  poly asin(int k = -1) const {
    const poly& x = *this;
    return (x.deriv() * (poly{1} - x * x).sqrt().inv()).integr();
  };
  poly atan(int k = -1) const {
    const poly& x = *this;
    return (x.deriv() * (poly{1} + x * x).inv()).integr();
  };

  friend std::ostream& operator<<(std::ostream& os, poly p) {
    os << "{";
    for (auto x : p) os << x() << " ";
    os << "}";
    return os;
  }

 private:
  poly& normalize() {
    while (this->size() && !this->back()) this->pop_back();
    return *this;
  }

  std::vector<T> evaluate_internal(std::vector<T>& xs,
                                   std::unique_ptr<product_tree>& rt) const {
    std::vector<T> res(xs.size());
    xs.resize(std::max(xs.size(), this->size()));
    std::function<void(product_tree*, poly)> solve = [&](product_tree* rt,
                                                         poly p) {
      p = p.modxk(rt->r - rt->l + 1);
      if (rt->l == rt->r) {
        if (rt->l < res.size()) res[rt->l] = p.front();
      } else {
        solve(rt->lson.get(), p.mulT(rt->rson->product));
        solve(rt->rson.get(), p.mulT(rt->lson->product));
      }
    };
    solve(rt.get(), this->mulT(rt->product.inv(xs.size())));
    return res;
  }
};

template <typename T>
class dft {
 public:
  static const bool use_fast_trans = true;
  static void trans(poly<T>& p) {
    assert(__builtin_popcount(p.size()) == 1);
    if constexpr (use_fast_trans) {
      dif(p);
    } else {
      bit_reverse(p);
      dit(p);
    }
  }

  static void inv_trans(poly<T>& p) {
    assert(__builtin_popcount(p.size()) == 1);
    if constexpr (use_fast_trans) {
      dit(p);
    } else {
      trans(p);
    }
    reverse(p.begin() + 1, p.end());
    T inv = T(p.size()).inv();
    for (T& x : p) x *= inv;
  }

  // should call dit after dif
  static void dit(poly<T>& p) {
    for (int len = 1; len < p.size(); len <<= 1) {
      auto sub_w = get_subw(len * 2);
      for (auto sub_p = p.begin(); sub_p != p.end(); sub_p += 2 * len)
        for (int i = 0; i < len; ++i) {
          T u = sub_p[i], v = sub_p[i + len] * sub_w[i];
          sub_p[i] = u + v;
          sub_p[i + len] = u - v;
        }
    }
  }

  static void dif(poly<T>& p) {
    for (int len = p.size() / 2; len >= 1; len >>= 1) {
      auto sub_w = get_subw(len * 2);
      for (auto sub_p = p.begin(); sub_p != p.end(); sub_p += 2 * len)
        for (int i = 0; i < len; ++i) {
          T _sub_pi = sub_p[i];
          sub_p[i] += sub_p[i + len];
          sub_p[i + len] = (_sub_pi - sub_p[i + len]) * sub_w[i];
        }
    }
  }

 private:
  typename std::vector<T>::iterator static get_subw(int len) {
    static std::vector<T> w = {0, 1};
    while (w.size() <= len) {
      T e[] = {1, T::root().pow((T::modulus() - 1) / w.size())};
      w.resize(w.size() * 2);
      for (int i = w.size() / 2; i < w.size(); ++i) w[i] = w[i / 2] * e[i & 1];
    }
    return w.begin() + len;
  }
};

template <typename T>
poly<T>& poly<T>::operator*=(poly<T> p) {
  if (this->empty() || p.empty()) return *this = {};
  constexpr int small_size = 128;
  if (this->size() < small_size || p.size() < small_size) {
    poly<T> t(this->size() + p.size() - 1);
    for (int i = 0; i < this->size(); i++)
      for (int j = 0; j < p.size(); j++) t[i + j] += this->at(i) * p[j];
    return *this = t;
  }
  int len = 1 << (std::__lg(this->deg() + p.deg()) + 1);
  this->resize(len);
  p.resize(len);
  dft<T>::trans(*this);
  dft<T>::trans(p);
  for (int i = 0; i < len; ++i) this->at(i) *= p[i];
  dft<T>::inv_trans(*this);
  return this->normalize();
}

}  // namespace polynomial

using poly = polynomial::poly<zint>;