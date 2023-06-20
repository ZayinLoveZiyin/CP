
std::stack<int> stk;
std::vector<int> ls(n, -1), rs(n, -1);
for (int i = 0; i < n; ++i) {
  for (; stk.size() && a[stk.top()] < a[i]; stk.pop()) {
    ls[i] = stk.top();
  }
  if (stk.size()) rs[stk.top()] = i;
  stk.push(i);
}

int rt = -1;
for (; stk.size(); stk.pop()) {
  rs[stk.top()] = rt;
  rt = stk.top();
}