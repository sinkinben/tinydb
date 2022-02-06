#include <stdlib.h>
#include "types.h"
#include "btree.h"
#include "table.h"
#ifndef CURSOR_H
#define CURSOR_H
cursor_t *table_start(table_t *table)
{
    cursor_t *cursor = table_find(table, 0);
    void *node = get_page(cursor->table->pager, cursor->page_num);
    uint32_t num_cells = *leaf_node_num_cells(node);
    cursor->end_of_table = (num_cells == 0 && *leaf_node_next_leaf(node) == 0);
    return cursor;
}

/* Whether if the node(page) that the cursor points to is empty. */
bool cursor_empty(cursor_t *cursor)
{
    void *page = get_page(cursor->table->pager, cursor->page_num);
    return *leaf_node_num_cells(page) == 0;
}

// get row address of the argument cursor
void *cursor_value(cursor_t *cursor)
{
    uint32_t page_num = cursor->page_num;
    void *page = get_page(cursor->table->pager, page_num);
    return leaf_node_value(page, cursor->cell_num);
}

// cursor++, points to next row of the table
// This need to be fixed when we implement b-tree
void cursor_advance(cursor_t *cursor)
{
    uint32_t page_num = cursor->page_num;
    void *node = get_page(cursor->table->pager, page_num);

    cursor->cell_num += 1;
    if (cursor->cell_num >= (*leaf_node_num_cells(node)))
    {
        uint32_t next_leaf = *leaf_node_next_leaf(node);
        if (next_leaf != 0)
        {
            cursor->cell_num = 0;
            cursor->page_num = next_leaf;
        }
        else
        {
            cursor->end_of_table = true;
        }
    }
}

#endif