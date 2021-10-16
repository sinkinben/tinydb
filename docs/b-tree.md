## B-Tree

> Why is a tree a good data structure for a database?
>
> - Searching for a particular value is fast (logarithmic time)
> - Inserting / deleting a value you’ve already found is fast (constant-ish time to rebalance)
> - Traversing a range of values is fast (unlike a hash map)



## 定义

B-tree 有 “阶 (Order)” 的概念，一般称为 $m$ 阶 B 树 ( a b-tree of order $m$ ) 。

B-tree 是基于二叉搜索树 (Binary Search Tree, BST) 扩展而来的，



**delete**

> Deletion works in the opposite way: the element is removed from the leaf. If the leaf becomes empty, a key is removed from the parent node. If that breaks invariant 3, the keys of the parent node and its immediate right (or left) sibling are reapportioned among them so that invariant 3 is satisfied. If this is not possible, the parent node can be combined with that sibling, removing a key another level up in the tree and possible causing a ripple all the way to the root. If the root has just two children, and they are combined, then the root is deleted and the new.

- 我们的 b+tree 只会在 leaf node 删除（因为删除数据总是在 leaf node 的）
- 假如删除了 leaf node 的 max key，需要递归调整 parent.cell[i].key
- 假如删除了若干行，导致某个 leaf node (page) 为空，暂不考虑页面回收。
  - 这意味着，虽然这一页是空的（没有任何的行数据），但还是会占用磁盘空间，并且**会导致永远不能被复用**（非常危险的行为）。
- 





## Refs

- [1] https://www.cs.cornell.edu/courses/cs3110/2012sp/recitations/rec25-B-trees/rec25.html
- [2] CLRS - Introduction to Algorithm (Chapter 18)