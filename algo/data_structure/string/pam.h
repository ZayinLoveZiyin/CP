template <size_t alphabet_size = 26>
class palindrome_automaton {
 public:
  struct node {
    std::array<int, alphabet_size> to;
    int link, len, count;

    explicit node(int len = 0) : len(len), link(-1), count(0) { to.fill(0); }
    explicit node(int len, int link) : len(len), link(link), count(0) {
      to.fill(0);
    }
  };

  palindrome_automaton() {
    int even_rt = newnode(0);
    int odd_rt = newnode(-1);
    nodes[even_rt].link = odd_rt;
    nodes[odd_rt].link = even_rt;
    last = even_rt;
  }

  void extend(char c) {
    text.push_back(c);
    int i = text.size() - 1;
    auto getlink = [&](int u) {
      while (i - nodes[u].len - 1 < 0 || text[i - nodes[u].len - 1] != c) {
        u = nodes[u].link;
      }
      return u;
    };
    int w = c - 'a';
    int u = getlink(last);
    if (!nodes[u].to[w]) {
      int v = newnode(nodes[u].len + 2, nodes[getlink(nodes[u].link)].to[w]);
      nodes[u].to[w] = v;
    }
    last = nodes[u].to[w];
    ++nodes[last].count;  // should be accmulated later from fail link tree
  }

  std::string to_string() const {
    std::ostringstream os;
    std::function<void(int, std::string)> travel = [&](int k, std::string s) {
      if (!k) return;
      os << k << ": " << s << " ~ " << nodes[k].count << "\n";
      for (int c = 0; c < alphabet_size; ++c)
        travel(nodes[k].to[c], std::string(1, c + 'a') + s + (char)(c + 'a'));
    };
    for (int c = 0; c < alphabet_size; ++c)
      travel(nodes[0].to[c], std::string(2, c + 'a'));
    for (int c = 0; c < alphabet_size; ++c)
      travel(nodes[1].to[c], std::string(1, c + 'a'));
    return os.str();
  }

 private:
  int last;
  std::string text;
  std::vector<node> nodes;

  template <typename... Args>
  int newnode(Args... args) {
    int res = nodes.size();
    nodes.push_back(node{args...});
    return res;
  }
};