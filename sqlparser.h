#include <string.h>
#include <stdlib.h>
#include "types.h"
#ifndef SQL_PARSER_H
#define SQL_PARSER_H
/**
 * This is equal to `SQL Compiler` module in sqlite.
 * Here, we just compiler(parse) the sql by hands :-D .
 * Parse the sql statements, and put the result in statement_t
 * From so far, we support sql statements:
 *   - insert
 *   - select
 *   - update
 **/
parse_result_t parse_row_fields(buffer_t *input, statement_t *statement)
{
    char *keyword = strtok(input->buffer, " ");
    char *id_str = strtok(NULL, " ");
    char *username = strtok(NULL, " ");
    char *email = strtok(NULL, " ");
    if (id_str == NULL || username == NULL || email == NULL)
    {
        return PARSE_SYNTAX_ERROR;
    }

    int id = atoi(id_str);
    if (strlen(username) > COLUMN_USERNAME_SIZE || strlen(email) > COLUMN_EMAIL_SIZE)
    {
        return PARSE_STRING_TOO_LONG;
    }

    statement->row_to_insert.id = id;
    strcpy(statement->row_to_insert.username, username);
    strcpy(statement->row_to_insert.email, email);
    return PARSE_SUCCESS;
}
parse_result_t parse_insert(buffer_t *input, statement_t *statement)
{
    statement->type = STATEMENT_INSERT;
    return parse_row_fields(input, statement);
}

parse_result_t parse_select(buffer_t *input, statement_t *statement)
{
    statement->type = STATEMENT_SELECT;
    return PARSE_SUCCESS;
}

parse_result_t parse_update(buffer_t *input, statement_t *statement)
{
    statement->type = STATEMENT_UPDATE;
    return parse_row_fields(input, statement);
}

parse_result_t parse_delete(buffer_t *input, statement_t *statement)
{
    return PARSE_UNIMPLEMENTED;
}

parse_result_t parse_commit(buffer_t *input, statement_t *statement)
{
    return PARSE_UNIMPLEMENTED;
}

parse_result_t parse_rollback(buffer_t *input, statement_t *statement)
{
    return PARSE_UNIMPLEMENTED;
}

// 执行 insert, select 命令
parse_result_t parse_statement(buffer_t *input, statement_t *statement)
{
    if (strncmp(input->buffer, "insert", 6) == 0)
    {
        return parse_insert(input, statement);
    }
    if (strncmp(input->buffer, "select", 6) == 0)
    {
        return parse_select(input, statement);
    }
    if (strncmp(input->buffer, "update", 6) == 0)
    {
        return parse_update(input, statement);
    }
    if (strncmp(input->buffer, "delete", 6) == 0)
    {
        return parse_delete(input, statement);
    }
    if (strncmp(input->buffer, "commit", 6) == 0)
    {
        return parse_commit(input, statement);
    }
    if (strncmp(input->buffer, "rollback", 8) == 0)
    {
        return parse_rollback(input, statement);
    }
    return PARSE_UNRECOGNIZED_STATEMENT;
}
#endif