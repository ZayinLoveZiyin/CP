#include <bits/stdc++.h>

#pragma region

namespace io {
struct convertor {
  template <typename, typename = void>
  struct is_iterable {
    static constexpr bool value = false;
  };
  template <typename T>
  struct is_iterable<T, std::void_t<decltype(std::declval<T>().begin()),
                                    decltype(std::declval<T>().end())>> {
    static constexpr bool value = true;
  };

  template <typename, typename = void>
  struct is_to_string_implemented {
    static constexpr bool value = false;
  };
  template <typename T>
  struct is_to_string_implemented<
      T, std::void_t<decltype(std::declval<T>().to_string())>> {
    static constexpr bool value = true;
  };

  template <typename F, typename S>
  static std::string to_string(const std::pair<F, S>& p) {
    return "(" + to_string(p.first) + ", " + to_string(p.second) + ")";
  }
  static std::string to_string(const std::string& s) { return '"' + s + '"'; }
  static std::string to_string(const char* s) {
    return '"' + std::string(s) + '"';
  }
  static std::string to_string(const char c) {
    return "'" + std::string(1, c) + "'";
  }
  static std::string to_string(const bool b) { return std::string(1, "FT"[b]); }
  template <typename AT,
            typename std::enable_if_t<std::is_arithmetic<AT>::value, int> = 0>
  static std::string to_string(const AT a) {
    return std::to_string(a);
  }

  template <typename VT,
            typename std::enable_if_t<is_iterable<VT>::value, int> = 0>
  static std::string to_string(const VT& v, const int indent_width = 0) {
    if (!v.size()) return std::string(indent_width, ' ') + "{}";
    std::string res;
    if constexpr (is_iterable<decltype(*v.begin())>::value) {
      res = "{\n";
      for (auto it = v.begin(); it != v.end(); ++it) {
        res += to_string(*it, indent_width + 1) + ",\n";
      }
    } else {
      res = std::string(indent_width, ' ') + "{";
      for (auto it = v.begin(); it != v.end(); ++it) {
        if (it != v.begin()) res += ", ";
        res += to_string(*it);
      }
    }
    res += "}";
    return res;
  }

  template <typename T, typename std::enable_if_t<
                            is_to_string_implemented<T>::value, int> = 0>
  static std::string to_string(T t) {
    return t.to_string();
  }

  template <typename T>
  static std::string to_string(
      T t, typename std::enable_if_t<!is_to_string_implemented<T>::value &&
                                         !std::is_arithmetic<T>::value &&
                                         !is_iterable<T>::value,
                                     int> = 0) {
    std::ostringstream os;
    os << t;
    return os.str();
  }
};

class ostream {
 public:
  ostream(FILE* file) : file(file) {}
  ~ostream() { reflesh(); }

  template <typename T>
  ostream& operator<<(T t) {
    write_single(t);
    return *this;
  }

  template <char... escapes>
  void log() {
    write_single('\n');
  }
  template <char... escapes, typename T, typename... Args>
  void log(T t, Args... args) {
    write_single(t);
    if (sizeof...(args)) (write_single(escapes), ...);
    log<escapes...>(args...);
  }
  template <typename... Args>
  void operator()(Args... args) {
    log<' '>(args...);
  }
  void flush() {
    reflesh();
    fflush(file);
  }
  void reflesh() {
    fwrite(buffer, sizeof(char), pointer - buffer, file);
    pointer = buffer;
  }

 private:
  void write_single(const char& c) {
    if (pointer == buffer + buffer_size) reflesh();
    *(pointer++) = c;
  }
  void write_single(const char* s) { write_single(std::string(s)); }
  void write_single(const std::string& s) {
    for (char c : s) write_single(c);
  }
  template <typename T>
  void write_single(T t) {
    for (char c : convertor::to_string(t)) write_single(c);
  }
  FILE* file;
  static const int buffer_size = 1 << 23;
  char buffer[buffer_size], *pointer = buffer;
};
ostream cout(stdout);
ostream cerr(stderr);
const char endl = '\n';

class istream : public std::istream {
 public:
  istream(std::istream& is) : is(is) {}
  template <typename T>
  istream& operator>>(T& t) {
    is >> t;
    return *this;
  }
  void operator()() {}
  template <typename T, typename... Args>
  void operator()(T& t, Args&... args) {
    is >> t;
    operator()(args...);
  }

  operator bool() { return (bool)is; }

 private:
  std::istream& is;
};
istream cin(std::cin);

class debug_helper {
 public:
  static bool isLeftBracket(char c) { return c == '[' || c == '(' || c == '{'; }
  static bool isRightBracket(char c) {
    return c == ']' || c == ')' || c == '}';
  }
  template <typename ostreamType, typename... Args>
  static void dump(ostreamType& os, std::string names, Args... args) {
    std::vector<std::string> val;
    (val.push_back(convertor::to_string(args)), ...);
    if (!names.size() || !val.size()) {
      os << '\n';
      os.reflesh();
      return;
    };

    std::vector<std::string> name{""};
    int count = 0;
    for (char c : names) {
      if (isspace(c)) continue;
      if (c == ',' && !count) {
        name.push_back("");
        continue;
      }
      if (isLeftBracket(c)) ++count;
      if (isRightBracket(c)) --count;
      name.back() += c;
    }
    assert(name.size() == val.size());
    std::unordered_map<std::string, std::string> mappings;
    for (int i = 0; i < name.size(); ++i) mappings[name[i]] = val[i];
    auto get_val = [&](std::string name) -> std::optional<std::string> {
      if (mappings.count(name)) return mappings[name];
      for (auto pr : mappings) {
        for (int i = 0; i + pr.first.size() <= name.size(); ++i) {
          if (name.substr(i, pr.first.size()) != pr.first) continue;
          if (i && std::isalnum(name[i - 1])) continue;
          if (i + 1 < name.size() && std::isalnum(name[i + 1])) continue;
          name.replace(i, pr.first.size(), pr.second);
        }
      }
      return get_expression_val(name);
    };
    std::function<std::string(std::string)> simplify =
        [&](std::string name) -> std::string {
      assert(mappings.count(name));
      std::stack<std::string> stk;
      stk.push("");
      bool should_add_mapping = true;
      for (char c : name) {
        if (isLeftBracket(c)) {
          stk.top() += c, stk.push("");
        } else if (isRightBracket(c) || c == ',') {
          std::optional<std::string> val = get_val(stk.top());
          if (!val.has_value()) {
            should_add_mapping = false;
            constexpr int kDisplayLength = 5;
            val = stk.top().size() > kDisplayLength ? "*" : stk.top();
          }
          stk.pop();
          stk.top() += val.value() + c;
          if (c == ',') stk.push("");
        } else {
          stk.top() += c;
        }
      }
      assert(stk.size() == 1);
      if (should_add_mapping) mappings[stk.top()] = mappings[name];
      return stk.top();
    };
    for (int i = 0; i < name.size(); ++i) {
      if (i) os << ", ";
      os << simplify(name[i]) << " = " << mappings.at(name[i]);
    }
    os << '\n';
    os.reflesh();
  }

 private:
  static std::optional<std::string> get_expression_val(std::string expression) {
    if (expression.empty()) return "";
    if (std::any_of(expression.begin(), expression.end(), [&](char c) {
          if ('0' <= c && c <= '9') return false;
          if (c == '+' || c == '-' || c == '*' || c == '(' || c == ')')
            return false;
          return true;
        }))
      return std::nullopt;
    std::stack<long long> num_stk;
    std::stack<long long> op_stk;
    auto get_precedence = [&](char c) {
      if (c == '#') return std::numeric_limits<int>::min();
      if (c == '(') return -2;
      if (c == ')') return -1;
      if (c == '+' || c == '-') return 0;
      if (c == '*') return 1;
    };
    auto get_op_result = [&](long long a, char op, long long b) {
      if (op == '+') return a + b;
      if (op == '-') return a - b;
      if (op == '*') return a * b;
    };
    expression += '#';
    for (int i = 0; i < expression.size(); ++i) {
      if (op_stk.size() == num_stk.size()) {
        long long sign = 1;
        if (!std::isalnum(expression[i])) {
          if (expression[i] == '+')
            sign = 1;
          else if (expression[i] == '-')
            sign = -1;
          else
            assert(0);
          ++i;
        }
        assert(std::isalnum(expression[i]));
        num_stk.push(0);
        for (; i < expression.size() && std::isalnum(expression[i]); ++i)
          num_stk.top() = num_stk.top() * 10 + expression[i] - '0';
        num_stk.top() = sign * num_stk.top();
        --i;
      } else if (expression[i] == '(') {
        op_stk.push('(');
      } else {
        while (op_stk.size() &&
               get_precedence(op_stk.top()) >= get_precedence(expression[i])) {
          long long b = num_stk.top();
          num_stk.pop();
          long long a = num_stk.top();
          num_stk.pop();
          long long res = get_op_result(a, op_stk.top(), b);
          op_stk.pop();
          num_stk.push(res);
        }
        if (expression[i] == '+' || expression[i] == '-' ||
            expression[i] == '*' || expression[i] == '#') {
          op_stk.push(expression[i]);
        } else if (expression[i] == ')') {
          assert(op_stk.top() == '(');
          op_stk.pop();
        }
      }
    }
    assert(num_stk.size() == 1);
    assert(op_stk.size() == 1 && op_stk.top() == '#');
    return std::to_string(num_stk.top());
  };
};

#define dbg_os io::cout
#define dbg(args...) io::debug_helper::dump(dbg_os, #args, ##args)
#define dbgf(name, args...)       \
  dbg_os << "[" << name << "]: "; \
  io::debug_helper::dump(dbg_os, #args, ##args)

#define single_dump(x) #x << " = " << x
#define concat_dump(left, right) left << ", " << right
#define dump_to_stream(args...) \
  dump_to_stream_id(args, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1)(args)
#define dump_to_stream_1(x1) single_dump(x1)
#define dump_to_stream_2(x1, x2) \
  concat_dump(single_dump(x1), dump_to_stream_1(x2))
#define dump_to_stream_3(x1, x2, x3) \
  concat_dump(single_dump(x1), dump_to_stream_2(x2, x3))
#define dump_to_stream_4(x1, x2, x3, x4) \
  concat_dump(single_dump(x1), dump_to_stream_3(x2, x3, x4))
#define dump_to_stream_5(x1, x2, x3, x4, x5) \
  concat_dump(single_dump(x1), dump_to_stream_4(x2, x3, x4, x5))
#define dump_to_stream_6(x1, x2, x3, x4, x5, x6) \
  concat_dump(single_dump(x1), dump_to_stream_5(x2, x3, x4, x5, x6))
#define dump_to_stream_7(x1, x2, x3, x4, x5, x6, x7) \
  concat_dump(single_dump(x1), dump_to_stream_6(x2, x3, x4, x5, x6, x7))
#define dump_to_stream_8(x1, x2, x3, x4, x5, x6, x7, x8) \
  concat_dump(single_dump(x1), dump_to_stream_7(x2, x3, x4, x5, x6, x7, x8))
#define dump_to_stream_9(x1, x2, x3, x4, x5, x6, x7, x8, x9) \
  concat_dump(single_dump(x1), dump_to_stream_8(x2, x3, x4, x5, x6, x7, x8, x9))
#define dump_to_stream_10(x1, x2, x3, x4, x5, x6, x7, x8, x9, x10) \
  concat_dump(single_dump(x1),                                     \
              dump_to_stream_9(x2, x3, x4, x5, x6, x7, x8, x9, x10))
#define dump_to_stream_id(x1, x2, x3, x4, x5, x6, x7, x8, x9, x10, x11, \
                          args...)                                      \
  dump_to_stream_##x11

}  // namespace io

#pragma endregion