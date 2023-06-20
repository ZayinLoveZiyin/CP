struct history_sum_tag {
  long long cnt_tag = 0;
  long long sum_tag = 0;
  long long hsum_tag = 0;
  void apply(const history_sum_tag& t) {
    hsum_tag += sum_tag * t.cnt_tag + t.hsum_tag;
    sum_tag += t.sum_tag;
    cnt_tag += t.cnt_tag;
  }
  friend std::ostream& operator<<(std::ostream& os, const history_sum_tag& t) {
    return os << "(" << t.cnt_tag << ", " << t.sum_tag << ", " << t.hsum_tag
              << ")";
  }
};

struct history_sum_info {
  long long len = 0;
  long long sum = 0;
  long long hsum = 0;
  friend history_sum_info operator+(const history_sum_info& lhs,
                                    const history_sum_info& rhs) {
    return history_sum_info{
        .len = lhs.len + rhs.len,
        .sum = lhs.sum + rhs.sum,
        .hsum = lhs.hsum + rhs.hsum,
    };
  }

  void apply(const history_sum_tag& t) {
    hsum += sum * t.cnt_tag + len * t.hsum_tag;
    sum += len * t.sum_tag;
  }

  friend std::ostream& operator<<(std::ostream& os, const history_sum_info& t) {
    return os << "(" << t.len << ", " << t.sum << ", " << t.hsum << ")";
  }
};