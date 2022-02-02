#include "types.h"
#include "cursor.h"
#include "transaction.h"
#include "sql-statement/statement.h"
#ifndef TINYDB_VM_H
#define TINYDB_VM_H

/**
 * The parsing result of a SQL statement is stored in statement_t.
 * These `execute_xxx` functions are equal to the "Vitrual Machine" in sqlite.
 * SQL Statement:
 * 1) execute_select, execute_delete are used by the dummy sql parser.
 *    > select (this will print all the rows)
 *    > delete [id] (this will delete the row whose id is `[id]` from B+Tree)
 *
 * 2) select_where, delete_where are used by the sql parser based on flex-bison.
 *    > select * from table where id > 10 and id < 30;
 *    > delete * from table where id > 10 and id < 30;
 **/

execute_result_t execute_insert(statement_t *statement, table_t *table)
{
    row_t *row_to_insert = &(statement->row_value);
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
#ifdef OPEN_TRANSACTION
    transaction_push(cursor, row_to_insert, statement->type);
#endif
    free(cursor);
    return EXECUTE_SUCCESS;
}

execute_result_t execute_select(statement_t *statement, table_t *table)
{
    cursor_t *cursor = table_start(table);
    row_t row;
    uint32_t i = 0;
    while (!(cursor->end_of_table))
    {
        deserialize_row(cursor_value(cursor), &row);
        i++;
        print_row(&row);
        cursor_advance(cursor);
    }
    printf("total %u rows\n", i);
    free(cursor);
    return EXECUTE_SUCCESS;
}

execute_result_t execute_select_where(statement_t *statement, table_t *table)
{
    cursor_t *cursor = table_start(table);
    row_t row;
    uint32_t cnt = 0;
    uint8_t flags = get_field_flags(statement->schemas);
    while (!(cursor->end_of_table))
    {
        deserialize_row(cursor_value(cursor), &row);

        if (test_condition(statement->conditions, &row))
        {
            cnt++;
            print_fields(&row, flags);
        }
        cursor_advance(cursor);
    }
    printf("total %u rows\n", cnt);
    free(cursor);
    return EXECUTE_SUCCESS;
}

execute_result_t execute_update(statement_t *statement, table_t *table)
{
    uint32_t key_to_update = statement->row_value.id;
    cursor_t *cursor = table_exists(table, key_to_update);
    if (cursor != NULL)
    {
        void *row = cursor_value(cursor);
#ifdef OPEN_TRANSACTION
        row_t temp;
        deserialize_row(row, &temp);
        transaction_push(cursor, &temp, statement->type);
#endif
        serialize_row(&(statement->row_value), row);
        free(cursor);
        return EXECUTE_SUCCESS;
    }
    return EXECUTE_NO_SUCH_KEY;
}

execute_result_t execute_delete(statement_t *statement, table_t *table)
{
    uint32_t key_to_delete = statement->row_value.id;
    cursor_t *cursor = table_exists(table, key_to_delete);
    if (cursor != NULL)
    {
        leaf_node_fake_delete(cursor, key_to_delete);
#ifdef OPEN_TRANSACTION
        row_t temp;
        deserialize_row(cursor_value(cursor), &temp);
        transaction_push(cursor, &temp, statement->type);
#endif
        free(cursor);
        return EXECUTE_SUCCESS;
    }
    return EXECUTE_NO_SUCH_KEY;
}

execute_result_t execute_delete_where(statement_t *statement, table_t *table)
{
    /* There is bugs while scanning B+Tree and deleting at the same time. */
    TODO(__FILE__, __LINE__);
    return EXECUTE_UNKNOWN_STATEMENT;
}

execute_result_t execute_commit(statement_t *statement, table_t *table)
{
    assert(statement->type == STATEMENT_COMMIT);
#ifdef OPEN_TRANSACTION
    transaction_commit();
#else
    transaction_fatal();
#endif
    return EXECUTE_SUCCESS;
}

execute_result_t execute_rollback(statement_t *statement, table_t *table)
{
    assert(statement->type == STATEMENT_ROLLBACK);
#ifdef OPEN_TRANSACTION
    transaction_rollback();
#else
    transaction_fatal();
#endif
    return EXECUTE_SUCCESS;
}

execute_result_t vm_executor(statement_t *statement, table_t *table)
{
    switch (statement->type)
    {
    case STATEMENT_INSERT:
        return execute_insert(statement, table);
    case STATEMENT_SELECT:
        return execute_select(statement, table);
    case STATEMENT_SELECT_WHERE:
        return execute_select_where(statement, table);
    case STATEMENT_UPDATE:
        return execute_update(statement, table);
    case STATEMENT_DELETE:
        return execute_delete(statement, table);
    case STATEMENT_DELETE_WHERE:
        return execute_delete_where(statement, table);
    case STATEMENT_COMMIT:
        return execute_commit(statement, table);
    case STATEMENT_ROLLBACK:
        return execute_rollback(statement, table);
    default:
        break;
    }
    return EXECUTE_UNKNOWN_STATEMENT;
}

void vm_logger(execute_result_t result, statement_t *statement, buffer_t *input)
{
    switch (result)
    {
    case EXECUTE_SUCCESS:
        printf("Executed.\n");
        break;
    case EXECUTE_DUPLICATE_KEY:
        printf("Execute Error: Duplicate key.\n");
        break;
    case EXECUTE_NO_SUCH_KEY:
        printf("Execute Error: No such key %u\n", statement->row_value.id);
        break;
    case EXECUTE_TABLE_FULL:
        printf("Execute Error: Table full.\n");
        break;
    case EXECUTE_UNKNOWN_STATEMENT:
        printf("Execute Error: Unknown sql statement '%s' \n", input->buffer);
        break;
    }
}

#endif
