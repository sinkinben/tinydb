#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "table.h"
#include "types.h"
#include "statement.h"
#include "debug.h"
#ifndef DATA_STRUCTURE_H
#define DATA_STRUCTURE_H

buffer_t *new_buffer_input()
{
    buffer_t *input_buffer = (buffer_t *)malloc(sizeof(buffer_t));
    input_buffer->buffer = NULL;
    input_buffer->buffer_length = 0;
    input_buffer->input_length = 0;
    return input_buffer;
}

void print_prompt()
{
    printf("tinydb > ");
}

void read_input(buffer_t *input)
{
    size_t bytes_read = getline(&(input->buffer), &(input->buffer_length), stdin);

    if (bytes_read <= 0)
    {
        printf("Error reading input\n");
        exit(EXIT_FAILURE);
    }
    // ignore '\n'
    input->input_length = bytes_read - 1;
    input->buffer[bytes_read - 1] = 0;
}

void close_input_buffer(buffer_t *input)
{
    free(input->buffer);
    free(input);
}

// 执行 meta 命令: .exit, etc
meta_command_result_t do_meta_command(buffer_t *input, table_t *table)
{
    if (strcmp(input->buffer, ".exit") == 0)
    {
        db_close(table);
        exit(EXIT_SUCCESS);
    }
    else if (strcmp(input->buffer, ".constants") == 0)
    {
        printf("Constants:\n");
        print_constants();
        return META_COMMAND_SUCCESS;
    }
    else
    {
        return META_COMMAND_UNRECOGNIZED_COMMAND;
    }
}

prepare_result_t prepare_insert(buffer_t *input, statement_t *statement)
{
    statement->type = STATEMENT_INSERT;

    char *keyword = strtok(input->buffer, " ");
    char *id_str = strtok(NULL, " ");
    char *username = strtok(NULL, " ");
    char *email = strtok(NULL, " ");

    if (id_str == NULL || username == NULL || email == NULL)
    {
        return PREPARE_SYNTAX_ERROR;
    }

    int id = atoi(id_str);
    if (strlen(username) > COLUMN_USERNAME_SIZE || strlen(email) > COLUMN_EMAIL_SIZE)
    {
        return PREPARE_STRING_TOO_LONG;
    }

    statement->row_to_insert.id = id;
    strcpy(statement->row_to_insert.username, username);
    strcpy(statement->row_to_insert.email, email);
    return PREPARE_SUCCESS;
}

// 执行 insert, select 命令
prepare_result_t prepare_statement(buffer_t *input, statement_t *statement)
{
    if (strncmp(input->buffer, "insert", 6) == 0)
    {
        return prepare_insert(input, statement);
    }
    if (strncmp(input->buffer, "select", 6) == 0)
    {
        statement->type = STATEMENT_SELECT;
        return PREPARE_SUCCESS;
    }
    return PREPARE_UNRECOGNIZED_STATEMENT;
}

execute_result_t execute_statement(statement_t *statement, table_t *table)
{
    switch (statement->type)
    {
    case STATEMENT_INSERT:
        return execute_insert(statement, table);
    case STATEMENT_SELECT:
        return execute_select(statement, table);
    default:
        break;
    }
    return EXECUTE_UNKNOWN_STATEMENT;
}

#endif