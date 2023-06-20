namespace kmp {

// s[0,next[i]]=s[i-next[i],i]
std::vector<int> getnext(const std::string& s) {
  std::vector<int> next(s.size());
  next[0] = -1;
  for (int i = 1, j = -1; i < s.size(); ++i) {
    while (~j && s[i] != s[j + 1]) j = next[j];
    if (s[i] == s[j + 1]) ++j;
    next[i] = j;
  }
  return next;
}

}  // namespace kmp