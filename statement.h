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
    serialize_row(row_to_insert, get_row_slot(table, table->num_rows));
    table->num_rows += 1;
    return EXECUTE_SUCCESS;
}

execute_result_t execute_select(statement_t *statement, table_t *table)
{
    row_t row;
    for (uint32_t i = 0; i < table->num_rows; i++)
    {
        deserialize_row(get_row_slot(table, i), &row);
        print_row(&row);
    }
    return EXECUTE_SUCCESS;
}

#endif