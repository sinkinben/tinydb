#include <stdlib.h>
#include "types.h"
#ifndef CURSOR_H
#define CURSOR_H
cursor_t *table_start(table_t *table)
{
    cursor_t *cursor = (cursor_t *)malloc(sizeof(cursor_t));
    cursor->table = table;
    cursor->row_num = 0;
    cursor->end_of_table = (table->num_rows == 0);
    return cursor;
}

cursor_t *table_end(table_t *table)
{
    cursor_t *cursor = (cursor_t *)malloc(sizeof(cursor_t));
    cursor->table = table;
    cursor->row_num = table->num_rows;
    cursor->end_of_table = true;
    return cursor;
}

// get row address of the argument cursor
void *cursor_value(cursor_t *cursor)
{
    uint32_t row_num = cursor->row_num;
    uint32_t page_num = row_num / ROWS_PER_PAGE;
    void *page = get_page(cursor->table->pager, page_num);
    uint32_t row_offset = row_num % ROWS_PER_PAGE;
    uint32_t bytes_offset = row_offset * ROW_SIZE;
    return page + bytes_offset;
}

// cursor++, points to next row of the table
void cursor_advance(cursor_t *cursor)
{
    // may be here need condition: (!cursor->end_of_table) 
    if (cursor->row_num < cursor->table->num_rows)
        cursor->row_num += 1;
    if (cursor->row_num >= cursor->table->num_rows)
    {
        cursor->end_of_table = true;
    }
}

#endif