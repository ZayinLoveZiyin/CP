使用方式为 `dbg(...)`，只要参数实现了 `to_string()` 方法或支持 `std::ostream` 输出流即可。

比较 fancy 的地方为可以带入参数，比如数组下标如果也出现在 `dbg` 参数里面，则可以直接带入，例如：

```c++
#include <bits/stdc++.h>
#include "dbg/dbg.h"

int main() {
  int i = 0;
  std::vector<int> a = {1, 2, 3, 4, 5};
  dbg(i, a, a[i]);
}
```

输出为

```
i = 0, a = {1, 2, 3, 4, 5}, a[0] = 1
```