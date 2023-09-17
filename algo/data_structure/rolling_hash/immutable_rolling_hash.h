namespace rolling_hash {

template <long long P, long long B>
struct rolling_hash_info {
  using mint = modular::mint<P>;
  rolling_hash_info() : hash(0), pwlen(1) {}
  rolling_hash_info(int x) : hash(x), pwlen(B) {}
  rolling_hash_info(mint h, mint pl) : hash(h), pwlen(pl) {}

  friend rolling_hash_info operator+(const rolling_hash_info& lhs,
                                     const rolling_hash_info& rhs) {
    return rolling_hash_info(lhs.hash * rhs.pwlen + rhs.hash,
                             lhs.pwlen * rhs.pwlen);
  }

  friend bool operator==(const rolling_hash_info& lhs,
                         const rolling_hash_info& rhs) {
    return lhs.hash == rhs.hash && lhs.pwlen == rhs.pwlen;
  }

  friend std::ostream& operator<<(std::ostream& os,
                                  const rolling_hash_info& info) {
    return os << "(" << info.hash << ", " << info.pwlen << ")";
  }

  mint hash, pwlen;
};

template <long long P, long long B>
struct immutable_rolling_hash {
  using mint = modular::mint<P>;

  immutable_rolling_hash(const std::string& s)
      : prefix_hash(s.size()), pwB(s.size()) {
    pwB[0] = 1;
    for (int i = 1; i < s.size(); ++i) pwB[i] = pwB[i - 1] * B;

    prefix_hash[0] = mint(s[0]);
    for (int i = 1; i < s.size(); ++i) {
      prefix_hash[i] = prefix_hash[i - 1] * B + s[i];
    }
  }

  mint get_prefix_hash(int p) {
    assert(-1 <= p && p < (int)prefix_hash.size());
    if (!~p) return 0;
    return prefix_hash[p];
  }

  rolling_hash_info<P, B> get_hash(int l, int r) {
    return rolling_hash_info<P, B>(
        get_prefix_hash(r) - get_prefix_hash(l - 1) * pwB[r - l + 1],
        pwB[r - l + 1]);
  }

  std::vector<mint> pwB, prefix_hash;
};

}  // namespace rolling_hash

using rolling_hash_info =
    rolling_hash::rolling_hash_info<(long long)1e18 + 9, (long long)1000>;
using immutable_rolling_hash =
    rolling_hash::immutable_rolling_hash<(long long)1e18 + 9, (long long)1000>;