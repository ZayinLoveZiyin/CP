template <size_t alphabet_size = 26>
class suffix_automaton {
 public:
  struct node {
    std::array<int, alphabet_size> to;
    int link, len, count;

    explicit node(int len = 0) : len(len), link(-1), count(0) { to.fill(-1); }
    explicit node(int len, int link, const std::array<int, alphabet_size>& to)
        : len(len), link(link), to(to), count(0) {}
  };

  suffix_automaton() : nodes() { newnode(); }

  explicit suffix_automaton(const std::string& s) : suffix_automaton() {
    insert(s);
  }

  void insert(const std::string& s) {
    nodes.reserve(size() + s.size() * 2);
    int last = 0;
    for (int i = 0; i < s.size(); ++i) {
      last = extend(last, s[i] - 'a');
    }
  }

  int extend(int k, int c) {
    if (~nodes[k].to[c] && nodes[nodes[k].to[c]].len == nodes[k].len + 1) {
      return nodes[k].to[c];
    }
    int leaf = newnode(nodes[k].len + 1);
    for (; ~k && !~nodes[k].to[c]; k = nodes[k].link) nodes[k].to[c] = leaf;
    if (!~k) {
      nodes[leaf].link = 0;
    } else {
      int p = nodes[k].to[c];
      if (nodes[k].len + 1 == nodes[p].len) {
        nodes[leaf].link = p;
      } else {
        int np = newnode(nodes[k].len + 1, nodes[p].link, nodes[p].to);
        nodes[p].link = nodes[leaf].link = np;
        for (; ~k && nodes[k].to[c] == p; k = nodes[k].link)
          nodes[k].to[c] = np;
      }
    }
    return leaf;
  }

  void build_ancestors() {
    for (int i = 1; i < size(); ++i) ancestors[i] = {nodes[i].link};
    for (int j = 1; (1 << j) < size(); ++j) {
      for (int i = 0; i < size(); ++i)
        if (~ancestors[i][j - 1]) {
          ancestors[i][j] = ancestors[ancestors[i][j - 1]][j - 1];
        } else {
          ancestors[i][j] = -1;
        }
    }
  }

  std::vector<int> mark_count(const std::string& s) {
    std::vector<int> ends;
    int k = 0;
    for (char c : s) {
      k = nodes[k].to[c - 'a'];
      assert(~k);
      ends.push_back(k);
      ++nodes[k].count;
    }
    return ends;
  }

  void addup_count() {
    std::vector<int> ids(size()), bucket(size());
    for (int i = 0; i < size(); ++i) ++bucket[nodes[i].len];
    for (int i = 1; i < bucket.size(); ++i) bucket[i] += bucket[i - 1];
    for (int i = 0; i < size(); ++i) ids[--bucket[nodes[i].len]] = i;
    for (int i = size() - 1; i; --i)
      nodes[nodes[ids[i]].link].count += nodes[ids[i]].count;
  }

  const node& operator[](int v) const { return nodes[v]; }

  int maxlen(int v) const { return nodes[v].len; }

  int minlen(int v) const { return v ? nodes[nodes[v].link].len + 1 : 0; }

  int size() const { return nodes.size(); }

  std::string to_string() const {
    std::ostringstream os;
    std::function<void(int, std::string)> travel = [&](int k, std::string s) {
      if (!~k) return;
      os << k << ": " << s << " ~ " << nodes[k].count << "\n";
      for (int c = 0; c < alphabet_size; ++c)
        travel(nodes[k].to[c], s + (char)(c + 'a'));
    };
    travel(0, "");
    return os.str();
  }

 private:
  std::vector<node> nodes;
  std::vector<std::vector<int>> ancestors;

  template <typename... Args>
  int newnode(Args... args) {
    int res = nodes.size();
    nodes.push_back(node{args...});
    return res;
  }
};