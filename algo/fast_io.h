namespace fastio {

class ostream;

class ostream_control_char {
 public:
  ostream_control_char(const auto& handler) : handler_(handler) {}

 private:
  const std::function<void(ostream&)> handler_;
  friend class ostream;
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

  ostream& operator<<(const ostream_control_char& oct) {
    oct.handler_(*this);
    return *this;
  }

  inline void flush() {
    reflesh();
    fflush(file);
  }
  inline void reflesh() {
    fwrite(buffer, sizeof(char), pointer - buffer, file);
    pointer = buffer;
  }

 private:
  inline void write_single(const char& c) {
    if (pointer == buffer + buffer_size) reflesh();
    *(pointer++) = c;
  }
  inline void write_single(const char* s) { write_single(std::string(s)); }
  inline void write_single(const std::string& s) {
    for (char c : s) write_single(c);
  }
  template <typename AT,
            typename std::enable_if_t<std::is_arithmetic<AT>::value, int> = 0>
  inline void write_single(const AT a) {
    return write_single(std::to_string(a));
  }

  FILE* file;
  static const int buffer_size = 1 << 23;
  char buffer[buffer_size], *pointer = buffer;
  friend class ostream_control_char;
};
ostream cout(stdout);

const ostream_control_char endl([](ostream& os) {
  os << '\n';
  os.flush();
});

class istream : public std::istream {
 public:
  istream(FILE* file) : file(file) {}
  template <typename AT,
            typename std::enable_if_t<std::is_arithmetic<AT>::value, int> = 0>
  istream& operator>>(AT& a) {
    AT abs_value = 0;
    int sign = 1;
    char c;
    do
      c = read_single();
    while (!std::isdigit(c) && c != '-');
    if (c == '-') {
      sign = -1;
      c = read_single();
    }
    for (; std::isdigit(c); c = read_single()) {
      abs_value = (abs_value << 3) + (abs_value << 1) + (c ^ '0');
    }
    a = sign * abs_value;
    return *this;
  }

  operator bool() { return !std::feof(file); }

 private:
  inline bool reflesh() {
    if (pointer == buffer + buffer_size) {
      if (!fread(buffer, sizeof(char), buffer_size, file)) return false;
      pointer = buffer;
    }
    return true;
  }

  inline char read_single() {
    if (!reflesh()) return EOF;
    return *(pointer++);
  }

  FILE* file;
  static const int buffer_size = 1 << 23;
  char buffer[buffer_size], *pointer = buffer;
};

istream cin(stdin);
}  // namespace fastio