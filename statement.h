#include "types.h"
#include "table.h"
#ifndef STATEMENT_H
#define STATEMENT_H

execute_result_t execute_insert(statement_t *statement, table_t *table)
{
    void *node = get_page(table->pager, table->root_page_num);
    uint32_t num_cells = *leaf_node_num_cells(node);
    if (num_cells >= LEAF_NODE_MAX_CELLS)
    {
        return EXECUTE_TABLE_FULL;
    }
    row_t *row_to_insert = &(statement->row_to_insert);
    uint32_t key_to_insert = row_to_insert->id;

    cursor_t *cursor = table_find(table, key_to_insert);

    if (cursor->cell_num < num_cells)
    {
        uint32_t key_at_index = *leaf_node_key(node, cursor->cell_num);
        if (key_at_index == key_to_insert)
        {
            return EXECUTE_DUPLICATE_KEY;
        }
    }

    leaf_node_insert(cursor, row_to_insert->id, row_to_insert);
    free(cursor);
    return EXECUTE_SUCCESS;
}

execute_result_t execute_select(statement_t *statement, table_t *table)
{
    cursor_t *cursor = table_start(table);
    row_t row;
    while (!(cursor->end_of_table))
    {
        deserialize_row(cursor_value(cursor), &row);
        print_row(&row);
        cursor_advance(cursor);
    }
    free(cursor);
    return EXECUTE_SUCCESS;
}

#endif