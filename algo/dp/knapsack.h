namespace knapsack {

// Computes all possible subset sums from 0 to m.
// Runs in O(m sqrt m / w) if the sum of `v` is bounded by m, and O(m^2 / w)
// otherwise.
template <int LEN = 1 << 20>
std::vector<int> possibleSubsetSum(int m, const std::vector<int>& v) {
  if (LEN > 1 && 2 * m < LEN) return possibleSubsetSum<LEN / 2>(m, v);
  assert(m < LEN);
  std::vector<int> freq(m + 1);
  for (int x : v)
    if (x <= m) ++freq[x];
  std::bitset<LEN> bits;
  std::vector<int> res = {0};
  bits.set(0);
  for (int x = 1; x <= m; ++x) {
    if (freq[x] >= 3) {
      int d = (freq[x] - 1) / 2;
      freq[x] -= 2 * d;
      if (2 * x <= m) freq[2 * x] += d;
    }
    while (freq[x]--) bits |= bits << x;
    if (bits[x]) res.push_back(x);
  }
  return res;
}

};  // namespace knapsack