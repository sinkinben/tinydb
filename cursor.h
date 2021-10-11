#include <stdlib.h>
#include "types.h"
#include "btree.h"
#ifndef CURSOR_H
#define CURSOR_H
cursor_t *table_start(table_t *table)
{
    cursor_t *cursor = (cursor_t *)malloc(sizeof(cursor_t));
    cursor->table = table;
    cursor->page_num = table->root_page_num;
    cursor->cell_num = 0;

    void *root_node = get_page(table->pager, table->root_page_num);
    uint32_t num_cells = *leaf_node_num_cells(root_node);
    cursor->end_of_table = (num_cells == 0);

    return cursor;
}

// cursor_t *table_end(table_t *table)
// {
//     cursor_t *cursor = (cursor_t *)malloc(sizeof(cursor_t));
//     cursor->table = table;
//     cursor->page_num = table->root_page_num; // Why?
//     void *root_node = get_page(table->pager, table->root_page_num);
//     uint32_t num_cells = *leaf_node_num_cells(root_node);
//     cursor->cell_num = num_cells;
//     cursor->end_of_table = true;
//     return cursor;
// }

// get row address of the argument cursor
void *cursor_value(cursor_t *cursor)
{
    uint32_t page_num = cursor->page_num;
    void *page = get_page(cursor->table->pager, page_num);
    return leaf_node_value(page, cursor->cell_num);
}

// cursor++, points to next row of the table
void cursor_advance(cursor_t *cursor)
{
    uint32_t page_num = cursor->page_num;
    void *node = get_page(cursor->table->pager, page_num);
    
    cursor->cell_num += 1;
    if (cursor->cell_num >= (*leaf_node_num_cells(node)))
    {
        cursor->end_of_table = true;
    }
}

#endif