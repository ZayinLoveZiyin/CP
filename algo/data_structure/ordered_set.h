
#include <ext/pb_ds/assoc_container.hpp>
#include <ext/pb_ds/tree_policy.hpp>
template <typename T>
using ordered_set =
    __gnu_pbds::tree<T, __gnu_pbds::null_type, std::less<T>,
                     __gnu_pbds::rb_tree_tag,
                     __gnu_pbds::tree_order_statistics_node_update>;

// rb_tree_tag 和 splay_tree_tag 选择树的类型(红黑树和伸展树)
// T:            自定义数据类型
// null_type:    无映射（老版本g++为null_mapped_type）
// less<T>:      Node的排序方式从小到大排序
// tree_order_statistics_node_update:  //参数表示如何更新保存节点信息
// tree_order_statistics_node_update会额外获得order_of_key()和find_by_order()两个功能。

ordered_set<Node> Tree;   // Node
                          // 自定义struct
                          // 注意重载less
Tree.insert(Node);        // 插入
Tree.erase(Node);         // 删除
Tree.order_of_key(Node);  // 求Node的排名:当前数小的数的个数
Tree.find_by_order(k);    // 返回排名为k的iterator 即有k个Node比*it小
Tree.join(b);  // 将b并入Tree，前提是两棵树类型一致并且二没有重复元素
Tree.split(v, b);  // 分裂，key小于等于v的元素属于Tree，其余属于b
Tree.lower_bound(Node);  // 返回第一个大于等于x的元素的迭代器
Tree.upper_bound(Node);  // 返回第一个大于x的元素的迭代器

//以上的所有操作的时间复杂度均为O(logn)
//注意，插入的元素会去重，如set
ordered_set<T>::point_iterator it = Tree.begin();  // 迭代器
//显然迭代器可以++，--运算