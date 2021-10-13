#include <stdint.h>
#include <string.h>
#include <assert.h>
#include "types.h"
#include "global.h"
#ifndef BTREE_H
#define BTREE_H
typedef enum
{
    NODE_INTERNAL,
    NODE_LEAF
} node_type_t;

/**
 * a leaf node <==> a page on disk
 * 每个 page 的开头需要存储一些 meta 信息
 * - node_type
 * - is_root
 * - parent_pointer
 * - num_cells: how many rows(cells) in our table
 * - cells: {key1, value1}, {key2, value2}, where key is actually "id" here
 **/

// Common Node Header Layout
const uint32_t NODE_TYPE_SIZE = sizeof(uint8_t);
const uint32_t NODE_TYPE_OFFSET = 0;

const uint32_t IS_ROOT_SIZE = sizeof(uint8_t);
const uint32_t IS_ROOT_OFFSET = NODE_TYPE_SIZE;

const uint32_t PARENT_POINTER_SIZE = sizeof(uint32_t);
const uint32_t PARENT_POINTER_OFFSET = IS_ROOT_OFFSET + IS_ROOT_SIZE;

const uint8_t COMMON_NODE_HEADER_SIZE = NODE_TYPE_SIZE + IS_ROOT_SIZE + PARENT_POINTER_SIZE;

// Leaf Node Header Layout
const uint32_t LEAF_NODE_NUM_CELLS_SIZE = sizeof(uint32_t);
const uint32_t LEAF_NODE_NUM_CELLS_OFFSET = COMMON_NODE_HEADER_SIZE;
const uint32_t LEAF_NODE_HEADER_SIZE = COMMON_NODE_HEADER_SIZE + LEAF_NODE_NUM_CELLS_SIZE;

// Leaf Node Body Layout
const uint32_t LEAF_NODE_KEY_SIZE = sizeof(uint32_t);
const uint32_t LEAF_NODE_KEY_OFFSET = 0;
const uint32_t LEAF_NODE_VALUE_SIZE = ROW_SIZE;
const uint32_t LEAF_NODE_VALUE_OFFSET = LEAF_NODE_KEY_OFFSET + LEAF_NODE_KEY_SIZE;
const uint32_t LEAF_NODE_CELL_SIZE = LEAF_NODE_KEY_SIZE + LEAF_NODE_VALUE_SIZE;
const uint32_t LEAF_NODE_SPACE_FOR_CELLS = PAGE_SIZE - LEAF_NODE_HEADER_SIZE;
const uint32_t LEAF_NODE_MAX_CELLS = LEAF_NODE_SPACE_FOR_CELLS / LEAF_NODE_CELL_SIZE;

// leaf node 已有 n = LEAF_NODE_MAX_CELLS  个 key
// 插入新 key 值时, 需要分裂
// 假如 n+1 是奇数, 默认分裂后的 left node 多一个节点
const uint32_t LEAF_NODE_RIGHT_SPLIT_COUNT = (LEAF_NODE_MAX_CELLS + 1) / 2;
const uint32_t LEAF_NODE_LEFT_SPLIT_COUNT = (LEAF_NODE_MAX_CELLS + 1) - LEAF_NODE_RIGHT_SPLIT_COUNT;

// Internal Node Header Layout
const uint32_t INTERNAL_NODE_NUM_KEYS_SIZE = sizeof(uint32_t);
const uint32_t INTERNAL_NODE_NUM_KEYS_OFFSET = COMMON_NODE_HEADER_SIZE;
const uint32_t INTERNAL_NODE_RIGHT_CHILD_SIZE = sizeof(uint32_t);
const uint32_t INTERNAL_NODE_RIGHT_CHILD_OFFSET = INTERNAL_NODE_NUM_KEYS_OFFSET + INTERNAL_NODE_NUM_KEYS_SIZE;
const uint32_t INTERNAL_NODE_HEADER_SIZE = COMMON_NODE_HEADER_SIZE + INTERNAL_NODE_NUM_KEYS_SIZE + INTERNAL_NODE_RIGHT_CHILD_SIZE;

/* Internal Node Body Layout
 *   - the child pointer (includes the right child pointer) is actually page number of a disk file
 *   - layout is like (child0, key0, child1, key1, ..., child[n-1], key[n-1], right_child)
 *   - there are n keys and n+1 child in internal nodes
 * Notice that our INTERNAL_NODE_HEADER_SIZE = 14, 
 * and total size of (child pointer, key) is just INTERNAL_NODE_CELL_SIZE = 8
 * which implies: 
 *   - each internal node can store (4096 - 14) / 8 = 510 pairs (child pointer, key),
 *   - plus the most right child, we can totally 510 keys, and 511 child pointer.
 * 
 * What does this means? Let's do a simple arithmetic.
 * 
 * +----------------------+-------------------+------------------------+
 * | interval node layers | max leaf nodes    | size of all leaf nodes |
 * +----------------------+-------------------+------------------------+
 * |          0           | 511 ^ 0 = 0       | 4KB                    |
 * |          1           | 511 ^ 1 = 511     | 4KB * 511 < 2MB        |
 * |          2           | 511 ^ 2 = 261,121 | 2MB * 511 < 1GB        |
 * |          3           | 511 ^ 3           | 1GB * 511 < 512GB      |
 * +----------------------+-------------------+------------------------+
 * 
 * This is why B-Tree is a useful data structure for index?
 *   - given a key, we can find the corresponding leaf node in log(n) time
 *   - in each node(internal/leaf), we can do a binary search, which is also log(n) time
 *   - we can search through 500GB of data by loading 4 pages from disk
 */
const uint32_t INTERNAL_NODE_KEY_SIZE = sizeof(uint32_t);
const uint32_t INTERNAL_NODE_CHILD_SIZE = sizeof(uint32_t);
const uint32_t INTERNAL_NODE_CELL_SIZE = INTERNAL_NODE_CHILD_SIZE + INTERNAL_NODE_KEY_SIZE;

// 函数声明
uint32_t *leaf_node_num_cells(void *);
uint32_t *internal_node_num_keys(void *);
uint32_t *internal_node_child(void *, uint32_t);
uint32_t get_node_max_key(void *);
uint32_t *internal_node_key(void *, uint32_t);
uint32_t *internal_node_right_child(void *);

bool is_root_node(void *node)
{
    return (bool)(*(uint8_t *)(node + IS_ROOT_OFFSET));
}

void set_node_root(void *node, bool is_root)
{
    *(uint8_t *)(node + IS_ROOT_OFFSET) = (uint8_t)is_root;
}

node_type_t get_node_type(void *node)
{
    uint8_t value = *(uint8_t *)(node + NODE_TYPE_OFFSET);
    assert(value == 0 || value == 1);
    return (node_type_t)value;
}

void set_node_type(void *node, node_type_t node_type)
{
    uint8_t value = node_type;
    uint8_t *pos = (uint8_t *)(node + NODE_TYPE_OFFSET);
    *pos = value;
}

void init_leaf_node(void *node)
{
    memset(node, 0, PAGE_SIZE);
    *leaf_node_num_cells(node) = 0;
    set_node_type(node, NODE_LEAF);
    set_node_root(node, false);
}

void init_internal_node(void *node)
{
    memset(node, 0, PAGE_SIZE);
    *internal_node_num_keys(node) = 0;
    set_node_type(node, NODE_INTERNAL);
    set_node_root(node, false);
}

// Until we start recycling free pages, new pages will always
// go onto the end of the database file
uint32_t get_unused_page_num(pager_t *pager)
{
    // 总是在文件末尾新增一页
    // TODO: 如果中间存在 delete 引起的空页, 那么应该优先返回这些空页
    return pager->num_pages;
}

void create_new_root(table_t *table, uint32_t right_child_page_num)
{
    /*
    - This function is called (called by `leaf_node_split_and_insert`) when we insert a new key 
      into the initialized root node, but the initialized root node is full.
    - The address of right child can be got by argument `right_child_page_num`.
    - The old root contains the data of left child after insertion, then we should:
      + create a new node, and copy data from old root
      + let the new one become the left child after insertion
      + let the old root become new root
    - Why do we let the new one become left child? This is a interesting question.
      + Consider we are spliting a leaf node (but not a root).
    */

    // now `root` is actually old root
    // it contains the data of left child (after insertion)
    void *root = get_page(table->pager, table->root_page_num);

    void *right_child = get_page(table->pager, right_child_page_num);

    // create a new page to left child
    uint32_t left_child_page_num = get_unused_page_num(table->pager);
    void *left_child = get_page(table->pager, left_child_page_num);

    // Left child has data copied from old root
    memcpy(left_child, root, PAGE_SIZE);
    set_node_root(left_child, false);

    // now, root become new root
    init_internal_node(root);
    set_node_root(root, true);

    // adjust children pointer of root
    *internal_node_num_keys(root) = 1;
    *internal_node_child(root, 0) = left_child_page_num;

    uint32_t left_child_max_key = get_node_max_key(left_child);
    *internal_node_key(root, 0) = left_child_max_key;
    *internal_node_right_child(root) = right_child_page_num;

    // now, done!
    // printf("here\n");
    // printf("left  node type: %d \n", get_node_type(left_child));
    // printf("right node type: %d \n", get_node_type(right_child));
    // printf("root  node type: %d \n", get_node_type(root));
}

uint32_t *leaf_node_num_cells(void *node)
{
    return (uint32_t *)(node + LEAF_NODE_NUM_CELLS_OFFSET);
}

void *leaf_node_cell(void *node, uint32_t cell_num)
{
    return (node + LEAF_NODE_HEADER_SIZE + cell_num * LEAF_NODE_CELL_SIZE);
}

uint32_t *leaf_node_key(void *node, uint32_t cell_num)
{
    return (uint32_t *)leaf_node_cell(node, cell_num);
}

void *leaf_node_value(void *node, uint32_t cell_num)
{
    return leaf_node_cell(node, cell_num) + LEAF_NODE_KEY_SIZE;
}

void leaf_node_split_and_insert(cursor_t *cursor, uint32_t key, row_t *value)
{
    /*
    - Create a new node and move half the cells over.
    - Insert the new value in one of the two nodes.
    - Update parent or create a new parent.
    */
    void *old_node = get_page(cursor->table->pager, cursor->page_num);

    uint32_t new_page_num = get_unused_page_num(cursor->table->pager);
    // this will increase cursor->table->pager->num_pages
    void *new_node = get_page(cursor->table->pager, new_page_num);
    init_leaf_node(new_node);

    /*
    - All existing keys plus new key should be divided
    - evenly between old (left) and new (right) nodes.
    - cursor->cell_num is the returned value of function `leaf_node_find`
    */

    // After insertion, new_node will be the right child, old_node will be the left child
    // I think these code is wonderful :-D!
    for (int32_t cell_idx = LEAF_NODE_MAX_CELLS; cell_idx >= 0; cell_idx--)
    {
        void *dest_node = cell_idx >= LEAF_NODE_LEFT_SPLIT_COUNT ? new_node : old_node;
        uint32_t index_within_node = cell_idx % LEAF_NODE_LEFT_SPLIT_COUNT;
        void *dest_ptr = leaf_node_cell(dest_node, index_within_node);

        // the operation below is similar to insert a new value into a sorted array
        // if `cell_idx` is the position we want to insert
        if (cell_idx == cursor->cell_num)
        {
            serialize_row(value, dest_ptr);
        }
        // move: idx-1 => idx
        else if (cell_idx > cursor->cell_num)
        {
            assert(cell_idx >= 1);
            memcpy(dest_ptr, leaf_node_cell(old_node, cell_idx - 1), LEAF_NODE_CELL_SIZE);
        }
        else /* if (cell_idx < cursor->cell_num) */
        {
            memcpy(dest_ptr, leaf_node_cell(old_node, cell_idx), LEAF_NODE_CELL_SIZE);
        }
    }

    // update cell_nums both in left child and right child
    *(leaf_node_num_cells(old_node)) = LEAF_NODE_LEFT_SPLIT_COUNT;
    *(leaf_node_num_cells(new_node)) = LEAF_NODE_RIGHT_SPLIT_COUNT;

    // now we should update their parent node
    // if `old_node` is a root node, then they have no existed parent => we should create a new one
    // otherwise, we add a "TODO" :-D
    if (is_root_node(old_node))
    {
        return create_new_root(cursor->table, new_page_num);
    }
    else
    {
        printf("Need to implement updating parent after spliting\n");
        TODO(__FILE__, __LINE__);
        exit(EXIT_FAILURE);
    }
}

void leaf_node_insert(cursor_t *cursor, uint32_t key, row_t *value)
{
    void *node = get_page(cursor->table->pager, cursor->page_num);

    uint32_t num_cells = *leaf_node_num_cells(node);

    // actually, num_cells == LEAF_NODE_MAX_CELLS
    if (num_cells >= LEAF_NODE_MAX_CELLS)
    {
        // current node(page) is full
        // printf("Need to implement splitting a leaf node.\n");
        // exit(EXIT_FAILURE);
        leaf_node_split_and_insert(cursor, key, value);
        return;
    }

    if (cursor->cell_num < num_cells)
    {
        for (uint32_t i = num_cells; i > cursor->cell_num; i--)
        {
            memcpy(leaf_node_cell(node, i), leaf_node_cell(node, i - 1), LEAF_NODE_CELL_SIZE);
        }
    }
    *(leaf_node_num_cells(node)) += 1;
    *(leaf_node_key(node, cursor->cell_num)) = key;
    serialize_row(value, leaf_node_value(node, cursor->cell_num));
}

/**
 * leaf node 中, key 也是有序的, 因此此处使用二分查找
 * Returns:
 * - the position of the key
 * - the position of another key that we’ll need to move if we want to insert the new key, or
 * - the position after the last key in the node (the position one past the last key)
 **/
cursor_t *leaf_node_find(table_t *table, uint32_t page_num, uint32_t key)
{
    void *node = get_page(table->pager, page_num);
    uint32_t num_cells = *leaf_node_num_cells(node);

    cursor_t *cursor = (cursor_t *)malloc(sizeof(cursor_t));
    cursor->table = table;
    cursor->page_num = page_num;

    // Binary Search: [l, r)
    uint32_t l = 0, r = num_cells;
    while (l != r)
    {
        uint32_t cell_num_index = l + ((r - l) >> 1);
        uint32_t key_at_index = *leaf_node_key(node, cell_num_index);
        if (key == key_at_index)
        {
            cursor->cell_num = cell_num_index;
            return cursor;
        }
        else if (key < key_at_index)
        {
            r = cell_num_index;
        }
        else if (key > key_at_index)
        {
            l = cell_num_index + 1;
        }
    }
    cursor->cell_num = l;
    return cursor;
}

uint32_t *internal_node_num_keys(void *node)
{
    return (uint32_t *)(node + INTERNAL_NODE_NUM_KEYS_OFFSET);
}

uint32_t *internal_node_right_child(void *node)
{
    return (uint32_t *)(node + INTERNAL_NODE_RIGHT_CHILD_OFFSET);
}

void *internal_node_cell(void *node, uint32_t cell_num)
{
    return (node + INTERNAL_NODE_HEADER_SIZE + INTERNAL_NODE_CELL_SIZE * cell_num);
}

uint32_t *internal_node_child(void *node, uint32_t cell_num)
{
    uint32_t num_keys = *internal_node_num_keys(node);
    if (cell_num > num_keys)
    {
        printf("[internal_node_child] Tried to access cell_num %d > num_keys %d\n", cell_num, num_keys);
        exit(EXIT_FAILURE);
    }
    else if (cell_num == num_keys)
    {
        return internal_node_right_child(node);
    }
    else
    {
        return internal_node_cell(node, cell_num);
    }
    // should not be here
    assert(0);
}

uint32_t *internal_node_key(void *node, uint32_t cell_num)
{
    return (void *)(internal_node_cell(node, cell_num)) + INTERNAL_NODE_CHILD_SIZE;
}

uint32_t get_node_max_key(void *node)
{
    // for each node, the last key is the max key
    node_type_t node_type = get_node_type(node);
    if (node_type == NODE_INTERNAL)
    {
        // if num_keys - 1 == 0xffffffff
        // then it will cause segment fault here, we should pay attention to this
        uint32_t num_keys = *internal_node_num_keys(node);
        assert(num_keys != 0);
        return *internal_node_key(node, num_keys - 1);
    }
    if (node_type == NODE_LEAF)
    {
        uint32_t num_cells = *leaf_node_num_cells(node);
        assert(num_cells != 0);
        return *leaf_node_key(node, num_cells - 1);
    }
    // should not be here
    assert(0);
    return -1;
}

#endif