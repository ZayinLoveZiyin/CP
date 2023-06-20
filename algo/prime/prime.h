namespace random {
static std::mt19937_64 engine(
    std::chrono::steady_clock::now().time_since_epoch().count());

template <typename T,
          typename std::enable_if_t<std::is_integral<T>::value, int> = 0>
T rand(T l, T r) {
  std::uniform_int_distribution<T> rng(l, r);
  return rng(engine);
}
}  // namespace random

namespace prime {
long long slow_mul(long long a, long long b, long long modulus) {
  long long k = (long double)a * b / modulus;
  long long res = a * b - k * modulus;
  while (res < 0) res += modulus;
  while (res >= modulus) res -= modulus;
  return res;
}

bool is_prime(long long x) {
  assert(x > 0);
  if (x == 1) return false;
  if (~x & 1) return x == 2;
  auto pow = [&](long long a, long long k) {
    long long res = 1;
    for (; k; k >>= 1) {
      if (k & 1) res = slow_mul(res, a, x);
      a = slow_mul(a, a, x);
    }
    return res;
  };
  int ctz = __builtin_ctzll(x - 1);
  auto miller_rabin = [&](long long a) {
    long long t = pow(a, (x - 1) >> ctz);
    for (int i = 0; i < ctz; ++i) {
      long long nt = slow_mul(t, t, x);
      if (nt == 1 && t != 1 && t != x - 1) return false;
      t = nt;
    }
    return t == 1;
  };
  static const std::vector<int> test_a = {2,  3,  5,  7,  11, 13,
                                          17, 19, 23, 29, 31, 37};
  for (int a : test_a) {
    if (x == a) return true;
    if (!miller_rabin(a)) return false;
  }
  return true;
}

// suppose x=prod pi^ei, return {pi,ei}
template <typename T,
          typename std::enable_if_t<std::is_integral<T>::value, int> = 0>
std::unordered_map<T, int> get_factorization(T x) {
  std::unordered_map<T, int> res;
  std::function<T(T)> pollard_rho = [&](T x) {
    while (1) {
      T c = random::rand<T>(0, x - 1);
      T x0 = random::rand<T>(1, x - 1), x1 = x0;
      T s = 1;
      for (int i = 1;; ++i) {
        x0 = (slow_mul(x0, x0, x) + c) % x;
        if (x0 == x1) break;
        s = slow_mul(s, std::abs(x0 - x1), x);
        T d = std::gcd(s, x);
        if (1 < d && d < x) return d;
        if (i > 1 && __builtin_popcount(i) == 1) x1 = x0;
      }
    }
  };
  std::function<void(T, int)> solve = [&](T x, int e) {
    if (x == 1) return;
    if (is_prime(x)) {
      res[x] += e;
      return;
    }
    T d = pollard_rho(x);
    int k = 0;
    for (; x % d == 0; x /= d) ++k;
    solve(x, e);
    solve(d, e * k);
  };
  solve(x, 1);
  return res;
}

template <typename T,
          typename std::enable_if_t<std::is_integral<T>::value, int> = 0>
std::vector<T> get_all_divisors(std::unordered_map<T, int> factorization) {
  std::vector<std::pair<T, int>> fv;
  for (auto [p, e] : factorization) fv.emplace_back(p, e);
  std::vector<T> res;
  std::function<void(int, T)> dfs = [&](int i, T d) {
    if (i == fv.size()) {
      res.push_back(d);
    } else {
      auto [p, max_e] = fv[i];
      for (int e = 0; e <= max_e; ++e) {
        dfs(i + 1, d);
        d *= p;
      }
    }
  };
  dfs(0, 1);
  std::sort(res.begin(), res.end());
  return res;
}

template <typename T,
          typename std::enable_if_t<std::is_integral<T>::value, int> = 0>
std::vector<T> get_all_divisors(T x) {
  return get_all_divisors(get_factorization(x));
}
}  // namespace prime