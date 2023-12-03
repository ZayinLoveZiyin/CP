namespace polynomial {

template <typename T>
class dft {
 public:
  static const bool use_fast_trans = true;
  static void trans(std::vector<T>& p) {
    assert(__builtin_popcount(p.size()) == 1);
    if constexpr (use_fast_trans) {
      dif(p);
    } else {
      bit_reverse(p);
      dit(p);
    }
  }

  static void inv_trans(std::vector<T>& p) {
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
  static void dit(std::vector<T>& p) {
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

  static void dif(std::vector<T>& p) {
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
    static const T primitive_root = T::primitive_root();
    while (w.size() <= len) {
      T e[] = {1, primitive_root.pow((T::modulus() - 1) / w.size())};
      w.resize(w.size() * 2);
      for (int i = w.size() / 2; i < w.size(); ++i) w[i] = w[i / 2] * e[i & 1];
    }
    return w.begin() + len;
  }
};

}  // namespace polynomial
