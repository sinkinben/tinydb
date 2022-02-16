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
  - 这意味着，虽然这一页是空的（没有任何的行数据），但还是会占用磁盘空间。
  - 但当我们下次插入同样的 key 时，它依然可以恢复在这一页上（相当于伪删除）。
- 



## B+Tree on Disk

如果我们要存储一棵二叉树，那么它的每个结点在磁盘上，应该是怎么存储的呢？

```text
+--------+--------+--------+--------+                 
|  page  | value  | left   | right  |
+--------+--------+--------+--------+
|  0     | 123    | 1      | 2      |
+--------+--------+--------+--------+
|  1     | 101    | nil    | nil    |
+--------+--------+--------+--------+
|  2     | 789    | nil    | 3      |
+--------+--------+--------+--------+
|  3     | 999    | nil    | nil    |
+--------+--------+--------+--------+
```

上面的内容，其实表示的是：

```text
    123
   /   \
101     789
           \
            999
```

但在 B+Tree 中，我们需要区分 Internal Node 和 Leaf Node，这 2 种节点的结构是不一样的。但有一点是一致的：总是使用一个 Page (4KB) 去存储一个结点。



**Leaf Node Structure**

<img src="https://cstack.github.io/db_tutorial/assets/images/leaf-node-format.png" style="width:80%"/>

| Offset |             Member             | Size(bytes) |                         Description                          |
| :----: | :----------------------------: | :---------: | :----------------------------------------------------------: |
|   0    |          `node_type`           |      1      |                  Internal node or leaf node                  |
|   1    |           `is_root`            |      1      |                        True or false                         |
|   2    |            `parent`            |      4      |                     Parent's page number                     |
|   6    |          `num_cells`           |      4      |     How many cells in this node, each cell is a k-v pair     |
|   10   |          `next_leaf`           |      4      | The sibling's page number (all leaf nodes in b+tree will form a linked list) |
|   14   | `cell[0] = <key[0], value[0]>` |    4 + X    | `key` is the primary key of the table, and the size of  `value` depends on the definition of row |
|  ...   |              ...               |     ...     |                       More cells here                        |
|        |          `cell[n-1]`           |    4 + X    |                                                              |
|  ...   |          unused bytes          |     ...     | there may be blank space left finally, which is not enough to store a `k-v` pair (i.e. unused bytes is less than `4+X` bytes). |



**Internal Node Structure**

<img src="https://cstack.github.io/db_tutorial/assets/images/internal-node-format.png" style="width:80%"/>

| Offset |             Member             | Size(bytes) |                         Description                          |
| :----: | :----------------------------: | :---------: | :----------------------------------------------------------: |
|   0    |          `node_type`           |      1      |                  Internal node or leaf node                  |
|   1    |           `is_root`            |      1      |                        True or false                         |
|   2    |            `parent`            |      4      |                     Parent's page number                     |
|   6    |          `num_cells`           |      4      | How many cells in this node, each cell is a `<child, key>` pair |
|   10   |       `rightmost_child`        |      4      | a page number, represent the rightmost child of a internal node |
|   14   | `cell[0] = <child[0], key[0]>` |      8      |              `child` is the child's page number              |
|   22   |              ...               |     ...     |                       more cells here                        |
|  ...   |          `cell[n-1]`           |      8      |                                                              |
|  ...   |          unused bytes          |     ...     |         Unused bytes is less strictly than 8 bytes.          |



## Refs

- [1] https://www.cs.cornell.edu/courses/cs3110/2012sp/recitations/rec25-B-trees/rec25.html
- [2] CLRS - Introduction to Algorithm (Chapter 18)