#include <stdint.h>
#include "types.h"
#include "declaration.h"
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

void leaf_node_insert(cursor_t *cursor, uint32_t key, row_t *value)
{
    void *node = get_page(cursor->table->pager, cursor->page_num);

    uint32_t num_cells = *leaf_node_num_cells(node);

    if (num_cells >= LEAF_NODE_MAX_CELLS)
    {
        // current node(page) is full
        printf("Need to implement splitting a leaf node.\n");
        exit(EXIT_FAILURE);
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

node_type_t get_node_type(void *node)
{
    uint8_t value = *(uint8_t *)(node + NODE_TYPE_OFFSET);
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
    *leaf_node_num_cells(node) = 0;
    set_node_type(node, NODE_LEAF);
}

#endif