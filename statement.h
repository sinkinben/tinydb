#include "types.h"
#include "cursor.h"
#ifndef STATEMENT_H
#define STATEMENT_H

/**
 * The parsing result of a SQL statement sentence is stored in statement_t.
 * These `execute_xxx` functions are equal to the "Vitrual Machine" in sqlite.
 * SQL Statement:
 * - insert id username email
 * - select
 * - update id username email
 **/

execute_result_t execute_insert(statement_t *statement, table_t *table)
{
    row_t *row_to_insert = &(statement->row_to_insert);
    uint32_t key_to_insert = row_to_insert->id;
    cursor_t *cursor = table_find(table, key_to_insert);

    void *node = get_page(table->pager, cursor->page_num);
    uint32_t num_cells = *leaf_node_num_cells(node);

    if (cursor->cell_num < num_cells)
    {
        uint32_t key_at_index = *leaf_node_key(node, cursor->cell_num);
        // printf("[execute insert] key_at_index = %d\n", key_at_index);
        if (key_at_index == key_to_insert)
        {
            free(cursor);
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

execute_result_t execute_update(statement_t *statement, table_t *table)
{
    uint32_t key_to_update = statement->row_to_insert.id;
    cursor_t *cursor = table_exists(table, key_to_update);
    if (cursor != NULL)
    {
        void *row = cursor_value(cursor);
        serialize_row(&(statement->row_to_insert), row);
        free(cursor);
        return EXECUTE_SUCCESS;
    }
    return EXECUTE_NO_SUCH_KEY;
}

execute_result_t execute_delete(statement_t *statement, table_t *table)
{
    uint32_t key_to_delete = statement->row_to_insert.id;
    cursor_t *cursor = table_exists(table, key_to_delete);
    if (cursor != NULL)
    {
        // printf("[execute_delete] page num: %u\n", cursor->page_num);
        // printf("[execute_delete] cell num: %u\n", cursor->cell_num);
        leaf_node_fake_delete(cursor, key_to_delete);
        free(cursor);
        return EXECUTE_SUCCESS;
    }
    return EXECUTE_NO_SUCH_KEY;
}

#endif