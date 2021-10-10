#include "types.h"
#include "table.h"
#ifndef STATEMENT_H
#define STATEMENT_H

execute_result_t execute_insert(statement_t *statement, table_t *table)
{
    if (table->num_rows >= TABLE_MAX_ROWS)
    {
        return EXECUTE_TABLE_FULL;
    }
    row_t *row_to_insert = &(statement->row_to_insert);
    cursor_t *cursor = table_end(table);
    // serialize_row(row_to_insert, get_row_slot(table, table->num_rows));
    serialize_row(row_to_insert, cursor_value(cursor));
    table->num_rows += 1;
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