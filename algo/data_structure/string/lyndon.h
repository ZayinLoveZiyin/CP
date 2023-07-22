#include <bits/stdc++.h>

namespace lyndon {

std::vector<int> getfactorization(const std::string &s) {
  std::vector<int> right_ends;
  for (int i = 0; i < s.length();) {
    int j = i, k = i + 1;
    for (; k < s.length() && s[j] <= s[k]; j++, k++)
      if (s[j] < s[k]) j = i - 1;
    while (i <= j) i += k - j, right_ends.push_back(i);
  }
  return right_ends;
}

} // namespace lyndon