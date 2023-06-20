constexpr long long inf32 = 1e9;
constexpr long long inf64 = 1e18;

struct line {
  long long k, b;
  line(long long k = inf32, long long b = inf64) : k(k), b(b) {}
  long long operator()(long long x) const { return k * x + b; }
  friend std::ostream& operator<<(std::ostream& os, line line) {
    return os << "(" << line.k << "," << line.b << ")";
  }
  std::string to_string() const {
    std::stringstream os;
    os << *this;
    return os.str();
  }
};

// T is a monotonic function, and two different T should have most one
// intersection
template <typename T, typename cmp = std::less<decltype(T().operator()(0))>>
class lichao_tree {
 public:
  using type = decltype(T().operator()(0));
  static_assert(std::is_integral<type>::value);
  static const type type_min =
      std::min(std::numeric_limits<type>::min(),
               std::numeric_limits<type>::max(), cmp());
  static const type type_max =
      std::max(std::numeric_limits<type>::min(),
               std::numeric_limits<type>::max(), cmp());

  lichao_tree(type domain_left, type domain_right, size_t maxsize = 0)
      : domain_left(domain_left), domain_right(domain_right) {
    assert(domain_left < domain_right);
    rt = -1;
    best.reserve(maxsize);
    ls.reserve(maxsize);
    rs.reserve(maxsize);
  }

  void insert(T f) { rt = insert(rt, domain_left, domain_right, f); }
  type query(type x) { return query(rt, domain_left, domain_right, x); }

 private:
  int newnode(T f) {
    int id = best.size();
    best.emplace_back(f);
    ls.push_back(-1);
    rs.push_back(-1);
    return id;
  }
  int insert(int rt, type l, type r, T f) {
    if (!~rt) {
      rt = newnode(f);
    } else {
      type mid = l + (r - l) / 2;
      if (cmp()(f(mid), best[rt](mid))) std::swap(best[rt], f);
      if (cmp()(f(l), best[rt](l))) ls[rt] = insert(ls[rt], l, mid, f);
      if (cmp()(f(r), best[rt](r))) rs[rt] = insert(rs[rt], mid + 1, r, f);
    }
    return rt;
  }

  type query(int rt, type l, type r, type x) {
    if (!~rt) return type_max;
    if (l == r) return best[rt](x);
    type mid = l + (r - l) / 2;
    if (x <= mid)
      return std::min(best[rt](x), query(ls[rt], l, mid, x), cmp());
    else
      return std::min(best[rt](x), query(rs[rt], mid + 1, r, x), cmp());
  }

  int rt;
  std::vector<T> best;
  std::vector<int> ls, rs;
  const type domain_left;
  const type domain_right;
};
