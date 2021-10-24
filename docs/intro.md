## Introduction

> - What format is data saved in? (in memory and on disk)
> - When does it move from memory to disk?
> - Why can there only be one primary key per table?
> - How does rolling back a transaction work?
> - How are indexes formatted?
> - When and how does a full table scan happen?
> - What format is a prepared statement saved in?

 sqlite 的架构：

<img src="https://cstack.github.io/db_tutorial/assets/images/arch1.gif" />

A query goes through a chain of components in order to retrieve or modify data. The **front-end** consists of the:

- tokenizer
- parser
- code generator

The input to the front-end is a SQL query. the output is sqlite virtual machine bytecode (essentially a compiled program that can operate on the database).



**准备**

- 只有一个 database 并且只有一个 `table(id, username, email)` 。
- 只有最简单的主键索引 `id` .
- 没有异常处理，如果发现错误/异常：
  - 一般使用 `assert` 捕获异常。
  - 简单 `printf` 后直接 `exit(FAIL)` .
  - 如果有 `errno` ，那么打印 `errno` 的信息。
- 采用测试驱动开发 (Test Driven Development)
  - 先把整个系统划分分为若干个里程碑（预期目标）
  - 针对每个里程碑设计若干测试用例
  - 编写代码，直到完全通过某一里程碑的测试，并撰写对应的文档。

> 在真实的业务系统中，一般来说写注释要比写文档要好，因为业务迭代过于频繁，文档更新常常跟不上业务的更新。



**系统里程碑**

- P0 - 使用 `ruby` 编写测试框架
  - 后续逐步添加测试，并针对里程碑进行改进
- P1 - 交互式命令行
  - meta command: `.exit, .debug` ，`.exit` 用于退出 `tinydb` ，`.debug` 用于打印数据/调试。
  - sql command: `insert, select` .
- P2 - 简单的 SQL Compiler
  - 先支持 `insert, select` 2 种 Statement，不支持 `where, group by` 等关键字。
  - `insert` 语句格式为: `insert [id] [username] [email]`  。
  - `select` 语句格式为: `select` ，默认输出 `table` 的所有数据。
  - 2 种 SQL Statement 分别通过 `parse_insert, parse_select` 来进行 parsing (其实这就是手工的 SQL Compiler 😅) 。
- P3 - 实现 `insert, select` 的功能
  - `table` 仅仅在内存当中，不支持持久化 (Persistence) 。
  - `table` 的每一行都是连续存放的，即通过数组的方式存储 `table` 数据。
  - 仅支持在 `table` 的末尾进行  `insert`  。
- P4 - 实现持久化
  - 在 `.exit` 命令执行时，对 `table` 进行持久化存储，写入磁盘文件。
  - 添加 `pager_t` 结构，负责管理内存中的 `table` 与磁盘的 `table` 的映射。
- P5 - 添加 `Cursor` 
  - 行操作统一通过 `cursor_t` 这一数据结构完成。
- P6 - 初始化 `btree`
  - 先支持只有一个结点的 `btree` （也是根结点）。
  - 修改 `table` 和 `pager` 的数据机构，使得 `table` 具有主键索引。
- P7 - 改进 `btree`
  - 设计 `leaf node` 和 `interval node` 的结构（也是磁盘上一个 Page 的头部结构）
  - 支持 `leaf node` 的二分查找，对于一个 `key` ，判断是否 Duplicate Key，并返回插入位置。
- P8 - 实现完整的 `btree`
  - 分裂 `leaf node` 成为 `interval node` 
  - 实现 `btree` 的递归查找，支持增删改查
- P9 - 实现 `commit, rollback`  
  - 实现单机事务 (transaction) , 也不考虑并发，假设我们的 tinydb 只有一个 client 😅

**Part 8**

- With the current format, each page stores only rows (no metadata) so it is pretty space efficient. Insertion is also fast because we just append to the end.
- However, **finding a particular row** can only be done by scanning the **entire table**.
- And if we want to **delete** a row, we have to **fill in the hole** by moving every row that comes after it.







**Note**

- SQL Parser
- Pager: map memory data to disk
- The conception of Cursor



**TODO**

- ✅ 实现 Linux Kernel 的侵入式容器（实现一个链表）。
  - 基于这个链表，实现一个 transaction log 链表，然后支持 commit/rollback 等操作。
  - 即实现 WAL (Write Ahead Log) 。
- ✅ 支持 `commit` ：把内存中修改过的内容持久化到磁盘（目前是 `.exit` 的时候覆盖写入）
- ✅ 支持 `rollback`
- ✅ 支持 `delete` ：删除某个 `key` .
  - 目前的 `delete` 是伪删除。
  - 目前只是简单从 leaf node 中删除，如果某一页被删空，不会调整 parent 指针，也不会重新调整 B+Tree 的结构。
  - 这意味着，即使某一行从表中删除，但它依旧会占用磁盘空间，但可以重新插入相同的 key。
- 实现页面回收算法
  - 假如某一页被删空了，这一页应该被回收，后续在 `get_unused_page_num` 可以重新分配。
- 假设内存不足，只能缓存 `n` 页，可以实现一个 LFU 算法。
  - 目前最多允许缓存 `TABLE_MAX_PAGES` 页，如果超过这个数量，会触发 assert 错误。
- 目前的 transaction 是一个全局单一的 transaction，即单机、单个 client 的事务。如果需要支持多个 client 的并发事务：
  - 一个想法是：在 insert/update/delete 操作层面加锁，保证内存缓存的一致性（自然磁盘上的数据也具有一致性），但这样并发就是个壳。
  - 或者：使用 2PL 协议。
  - 或者：每个 client 都有一份独立的缓存，直接在自己的缓存上读写，当 client 执行 commit 的时候，才正式把修改的内容提交到 tinydb 的主节点（或者是直接提交落盘）。
    - 这就相当于是一个分布式架构的数据库了。
    - 但这样需要自己实现一个 Consistency Model ，脑洞有点大，并且不好证明其正确性（当然，实现难度有点大 😅 ，Consistency Model 我就没搞明白几个）。