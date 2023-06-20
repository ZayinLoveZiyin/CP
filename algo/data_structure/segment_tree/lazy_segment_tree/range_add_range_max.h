constexpr int inf = 1e9;

struct range_add_range_max_tag {
  int tag;
  range_add_range_max_tag(int t = 0) : tag(t) {}
  void apply(const range_add_range_max_tag& t) { tag += t.tag; }
  friend std::ostream& operator<<(std::ostream& os,
                                  const range_add_range_max_tag& t) {
    return os << "tag(" << t.tag << ")";
  }
};

struct range_add_range_max_info {
  int max, pos;
  range_add_range_max_info(int mx = -inf, int p = -1) : max(mx), pos(p) {}
  friend range_add_range_max_info operator+(
      const range_add_range_max_info& lhs,
      const range_add_range_max_info& rhs) {
    if (lhs.max > rhs.max) return lhs;
    return rhs;
  }

  void apply(const range_add_range_max_tag& t) { max += t.tag; }

  friend std::ostream& operator<<(std::ostream& os,
                                  const range_add_range_max_info& info) {
    return os << "info(" << info.max << ", " << info.pos << ")";
  }
};