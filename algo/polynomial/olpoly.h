namespace online_polynomial {

#define DISALLOW_COPY_AND_MOVE(className) \
  DISALLOW_COPY(className);               \
  DISALLOW_MOVE(className);

#define DISALLOW_COPY(className)        \
  className(const className&) = delete; \
  className& operator=(const className&) = delete;

#define DISALLOW_MOVE(className)   \
  className(className&&) = delete; \
  className& operator=(class className&&) = delete;

template <typename T>
class polymulstream;

template <typename T>
class polystream {
 public:
  polystream() = default;
  DISALLOW_COPY_AND_MOVE(polystream);

  virtual std::string source() { return "default"; }

  virtual void await(int k) {
    while (poly.size() <= k) poly.push_back(next());
  }

  virtual T get(int k) {
    await(k);
    return poly[k];
  }

  std::vector<T> range(int l, int r) {
    await(r - 1);
    return std::vector<T>(poly.begin() + l, poly.begin() + r);
  }

  T operator[](int k) { return get(k); }

 protected:
  virtual T next() {
    // need to be override or directly invoke get(int)
    assert(false);
    return 0;
  };

  // return the dif array of poly[len/2, len) with len/2 zeros appended
  virtual const std::vector<T>& get_dif_cache(int len) {
    assert(len >= 2);
    assert((len & (len - 1)) == 0);
    await(len - 1);
    int idx = std::__lg(len) - 1;
    while (dif_cache.size() <= idx) {
      int l = 1 << dif_cache.size();
      assert(l * 2 <= len);
      dif_cache.emplace_back(poly.begin() + l, poly.begin() + l * 2);
      dif_cache.back().resize(l * 2, T(0));
      polynomial::dft<T>::trans(dif_cache.back());
    }
    return dif_cache[idx];
  }

  friend class polymulstream<T>;

  std::vector<T> poly;
  std::vector<std::vector<T>> dif_cache;
};

template <typename T>
class polyfixedstream : public polystream<T> {
 public:
  polyfixedstream(const std::vector<T>& p) : p(p) {}
  DISALLOW_COPY_AND_MOVE(polyfixedstream);

  virtual std::string source() override { return "fixed"; }

  T next() override {
    int n = this->poly.size();
    return n < p.size() ? p[n] : T(0);
  }

 private:
  const std::vector<T> p;
};

template <typename T>
class polymulstream : public polystream<T> {
 public:
  polymulstream(std::shared_ptr<polystream<T>> a,
                std::shared_ptr<polystream<T>> b)
      : a(a), b(b) {}
  DISALLOW_COPY_AND_MOVE(polymulstream);

  virtual std::string source() override { return "multiplication"; }

  T next() override {
    int n = this->poly.size();
    update_contribution(n);
    if (!n) {
      contribution[0] += a->get(0) * b->get(0);
    } else {
      if (T a0 = a->get(0); a0) contribution[n] += a0 * b->get(n);
      if (T b0 = b->get(0); b0) contribution[n] += b0 * a->get(n);
    }
    return contribution[n];
  }

 private:
  void update_contribution(int n) {
    if (int len = 4 << std::__lg(n + 1); contribution.size() <= len)
      contribution.resize(len);
    for (int len = 1; 2 * len < n; len <<= 1) {
      constexpr int small_size = 32;
      if (len <= small_size) {
        int st = n - len;
        for (int i = 0; i < len; ++i)
          for (int j = 0; j < len; ++j) {
            contribution[n + i + j] += a->get(st + i) * b->get(len + j) +
                                       a->get(len + i) * b->get(st + j);
          }
      } else {
        std::vector<T> ta = a->range(n - len, n);
        std::vector<T> tb = b->range(n - len, n);
        ta.resize(len * 2);
        tb.resize(len * 2);
        polynomial::dft<T>::trans(ta);
        polynomial::dft<T>::trans(tb);
        const auto& a_dif_cache = a->get_dif_cache(len * 2);
        const auto& b_dif_cache = b->get_dif_cache(len * 2);
        std::vector<T> res(len * 2);
        for (int i = 0; i < res.size(); ++i)
          res[i] = a_dif_cache[i] * tb[i] + b_dif_cache[i] * ta[i];
        polynomial::dft<T>::inv_trans(res);
        for (int i = 0; i < res.size(); ++i) contribution[n + i] += res[i];
      }
      if (n & len) break;
    }

    if (n > 1 && (n & (n - 1)) == 0) {
      const auto& a_dif_cache = a->get_dif_cache(n);
      const auto& b_dif_cache = b->get_dif_cache(n);
      std::vector<T> res(n);
      for (int i = 0; i < n; ++i) res[i] = a_dif_cache[i] * b_dif_cache[i];
      polynomial::dft<T>::inv_trans(res);
      for (int i = 0; i < n; ++i) contribution[n + i] += res[i];
    }
  }
  std::vector<T> contribution;
  std::shared_ptr<polystream<T>> a, b;
};

template <typename T>
class polycornerstream : public polystream<T> {
 public:
  polycornerstream(std::shared_ptr<polystream<T>> ps, const std::vector<T>& p)
      : ps(ps), p(p) {}
  DISALLOW_COPY_AND_MOVE(polycornerstream);

  virtual std::string source() override { return "corner"; }

  T next() override {
    int n = this->poly.size();
    return n < p.size() ? p[n] : ps->get(n);
  }

 private:
  std::shared_ptr<polystream<T>> ps;
  const std::vector<T> p;
};

template <typename T>
class polyshlstream : public polystream<T> {
 public:
  polyshlstream(std::shared_ptr<polystream<T>> ps, long long k)
      : ps(ps), k(k) {}
  DISALLOW_COPY_AND_MOVE(polyshlstream);

  virtual std::string source() override { return "shl"; }

  T next() override { return ps->get(this->poly.size() + k); }

 private:
  std::shared_ptr<polystream<T>> ps;
  long long k;
};

template <typename T>
class polyshrstream : public polystream<T> {
 public:
  polyshrstream(std::shared_ptr<polystream<T>> ps, long long k)
      : ps(ps), k(k) {}
  DISALLOW_COPY_AND_MOVE(polyshrstream);

  virtual std::string source() override { return "shr"; }

  T next() override {
    int n = this->poly.size();
    return n >= k ? ps->get(n - k) : T(0);
  }

 private:
  std::shared_ptr<polystream<T>> ps;
  long long k;
};

template <typename T>
class polytransformstream : public polystream<T> {
 public:
  polytransformstream(std::shared_ptr<polystream<T>> ps,
                      std::function<T(int, T)> transformer)
      : ps(ps), transformer(transformer) {}
  DISALLOW_COPY_AND_MOVE(polytransformstream);

  virtual std::string source() override { return "transform"; }

  T next() override {
    int n = this->poly.size();
    return transformer(n, ps->get(n));
  }

 private:
  std::shared_ptr<polystream<T>> ps;
  std::function<T(int, T)> transformer;
};

template <typename T>
class polyscalestream : public polytransformstream<T> {
 public:
  polyscalestream(std::shared_ptr<polystream<T>> ps, T cof)
      : polytransformstream<T>(ps, [cof](int n, T an) { return an * cof; }) {}
  DISALLOW_COPY_AND_MOVE(polyscalestream);

  virtual std::string source() override { return "scale"; }
};

template <typename T>
class polyaddstream : public polytransformstream<T> {
 public:
  polyaddstream(std::shared_ptr<polystream<T>> a,
                std::shared_ptr<polystream<T>> b)
      : polytransformstream<T>(a, [b](int n, T an) { return an + b->get(n); }) {
  }
  DISALLOW_COPY_AND_MOVE(polyaddstream);

  virtual std::string source() override { return "addition"; }
};

template <typename T>
class polyminusstream : public polytransformstream<T> {
 public:
  polyminusstream(std::shared_ptr<polystream<T>> a,
                  std::shared_ptr<polystream<T>> b)
      : polytransformstream<T>(a, [b](int n, T an) { return an - b->get(n); }) {
  }
  DISALLOW_COPY_AND_MOVE(polyminusstream);

  virtual std::string source() override { return "subtraction"; }
};

template <typename T>
class polyderivstream : public polystream<T> {
 public:
  polyderivstream(std::shared_ptr<polystream<T>> ps) : ps(ps) {}
  DISALLOW_COPY_AND_MOVE(polyderivstream);

  virtual std::string source() override { return "derivation"; }

  T next() override {
    int n = this->poly.size();
    return ps->get(n + 1) * (n + 1);
  }

 private:
  std::shared_ptr<polystream<T>> ps;
};

template <typename T>
class polyintegralstream : public polystream<T> {
 public:
  polyintegralstream(std::shared_ptr<polystream<T>> ps, T f0 = 0)
      : ps(ps), f0(f0) {}
  DISALLOW_COPY_AND_MOVE(polyintegralstream);

  virtual std::string source() override { return "integral"; }

  T next() override {
    int n = this->poly.size();
    return n ? ps->get(n - 1) / n : f0;
  }

 private:
  std::shared_ptr<polystream<T>> ps;
  T f0;
};

template <typename T>
class polybackfillstream : public polystream<T> {
 public:
  polybackfillstream() {}
  polybackfillstream(const std::shared_ptr<polystream<T>>& ps) : ps(ps) {}

  virtual std::string source() override { return "backfill"; }

  void backfill(const std::shared_ptr<polystream<T>>& ps_) {
    assert(!established);
    assert(ps_);
    ps = ps_;
  }

  T next() override {
    established = true;
    assert(ps);
    return ps->get(this->poly.size());
  }

 private:
  bool established = false;
  std::shared_ptr<polystream<T>> ps;
};

template <typename T>
class polyinputstream : public polystream<T> {
 public:
  polyinputstream(int n) : seq(n) {}
  DISALLOW_COPY_AND_MOVE(polyinputstream);

  virtual std::string source() override { return "input"; }

  T next() override {
    int n = this->poly.size();
    return n < seq.size() ? seq[n] : T(0);
  }

  T& get_mutable(int k) {
    if (seq.size() <= k) {
      std::cerr << "[warning]: resizing polyinputstream\n";
      seq.resize(k + 1);
    }
    return seq[k];
  }

 private:
  std::vector<T> seq;
};

template <typename T>
class polycustomstream : public polystream<T> {
 public:
  polycustomstream(std::function<T(int)> handler) : handler(handler) {}
  DISALLOW_COPY_AND_MOVE(polycustomstream);

  virtual std::string source() override { return "custom"; }

  T next() override {
    int n = this->poly.size();
    return handler(n);
  }

 private:
  std::function<T(int)> handler;
};

template <typename T>
struct olpoly {
  std::shared_ptr<polystream<T>> ps;
  olpoly() : ps(std::make_shared<polybackfillstream<T>>()) {}
  olpoly(const olpoly& olp) : ps(olp.ps) {}
  explicit olpoly(int n) : ps(std::make_shared<polyinputstream<T>>(n)) {}
  explicit olpoly(const std::vector<T>& p)
      : ps(std::make_shared<polyfixedstream<T>>(p)) {}
  explicit olpoly(std::function<T(int)> handler)
      : ps(std::make_shared<polycustomstream<T>>(handler)) {}
  explicit olpoly(const std::shared_ptr<polystream<T>>& ps) : ps(ps) {}

  std::string source() const { return ps->source(); }
  T get(int n) const { return ps->get(n); }
  void set(int n, T x) const {
    auto* ptr = dynamic_cast<polyinputstream<T>*>(ps.get());
    assert(ptr);
    return ptr->get_mutable(n) = x;
  }
  T& get_mutable(int n) {
    auto* ptr = dynamic_cast<polyinputstream<T>*>(ps.get());
    assert(ptr);
    return ptr->get_mutable(n);
  }

  void backfill(const olpoly& olp) {
    auto* ptr = dynamic_cast<polybackfillstream<T>*>(ps.get());
    assert(ptr);
    ptr->backfill(olp.ps);
  }

  olpoly& operator=(const olpoly& olp) {
    backfill(olp);
    return *this;
  }

  std::vector<T> range(int l, int r) { ps->range(l, r); }

  T operator()(int k) const { return get(k); }
  T& operator[](int k) { return get_mutable(k); }

  friend olpoly operator+(const olpoly& lhs, const olpoly& rhs) {
    return make_olpoly<polyaddstream<T>>(lhs.ps, rhs.ps);
  }
  friend olpoly operator-(const olpoly& lhs, const olpoly& rhs) {
    return make_olpoly<polyminusstream<T>>(lhs.ps, rhs.ps);
  }
  friend olpoly operator*(const olpoly& lhs, const olpoly& rhs) {
    return make_olpoly<polymulstream<T>>(lhs.ps, rhs.ps);
  }
  friend olpoly operator*(const olpoly& olp, const T& c) {
    return make_olpoly<polyscalestream<T>>(olp.ps, c);
  }
  friend olpoly operator*(const T& c, const olpoly& olp) {
    return make_olpoly<polyscalestream<T>>(olp.ps, c);
  }
  friend olpoly operator/(const olpoly& olp, const T& c) {
    return olp * c.inv();
  }
  friend olpoly operator/(const olpoly& lhs, const olpoly& rhs) {
    return lhs * rhs.inv();
  }

  olpoly transform(std::function<T(int, T)> transformer) const {
    return make_olpoly<polytransformstream<T>>(ps, transformer);
  }
  olpoly dot(const olpoly& olp) const {
    return make_olpoly<polytransformstream<T>>(
        ps, [b = olp.ps](int n, T an) { return an * b->get(n); });
  }
  olpoly dotdiv(const olpoly& olp) const {
    return make_olpoly<polytransformstream<T>>(
        ps, [b = olp.ps](int n, T an) { return an / b->get(n); });
  }
  olpoly shl(long long k = 1) const {
    return make_olpoly<polyshlstream<T>>(ps, k);
  }
  olpoly shr(long long k = 1) const {
    return make_olpoly<polyshrstream<T>>(ps, k);
  }
  olpoly corner(const std::vector<T>& p) const {
    return make_olpoly<polycornerstream<T>>(ps, p);
  }
  olpoly deriv() const { return make_olpoly<polyderivstream<T>>(ps); }
  olpoly integr(T c = 0) const {
    return make_olpoly<polyintegralstream<T>>(ps, c);
  }

  // usually use to avoid deadlock when calculateing fn with f0*fn
  olpoly ignore(long long k = 1) const { return corner(std::vector<T>(k, 0)); }

  olpoly sqr() const { return (*this) * (*this); }

  olpoly inv() const {
    assert(this->get(0));
    T inv0 = this->get(0).inv();
    olpoly res;
    res = (-inv0 * this->ignore() * res).corner({inv0});
    return res;
  }

  olpoly sqrt() const {
    T sqrt0 = this->get(0).sqrt();
    olpoly res;
    res = (((*this) - res.ignore().sqr()) / (sqrt0 * 2)).corner({sqrt0});
    return res;
  }

  olpoly ln() const {
    assert(this->get(0) == 1);
    return (this->deriv() / *this).integr();
  }

  olpoly exp() const {
    assert(this->get(0) == 0);
    olpoly res;
    res = (res * this->deriv()).integr(1);
    return res;
  }

  olpoly pow(long long k) const {
    int i = 0;
    while (!this->get(i)) ++i;
    olpoly t = this->shl(i) / this->get(i);
    olpoly res;
    res = (res * t.deriv() / t * k).integr(this->get(i).pow(k));
    return res.shr(k * i);
  }

  static olpoly fixed(const std::vector<T>& p) {
    return make_olpoly<polyfixedstream<T>>(p);
  }

 private:
  template <typename PolyStream, typename... Args>
  static std::shared_ptr<PolyStream> make_stream(const Args&... args) {
    return std::make_shared<PolyStream>(args...);
  }
  template <typename PolyStream, typename... Args>
  static olpoly make_olpoly(const Args&... args) {
    return olpoly(make_stream<PolyStream>(args...));
  }
};

}  // namespace online_polynomial

using olpoly = online_polynomial::olpoly<mint>;
