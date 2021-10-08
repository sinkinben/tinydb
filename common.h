#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#ifndef DATA_STRUCTURE_H
#define DATA_STRUCTURE_H

typedef struct
{
    char *buffer;
    size_t buffer_length;
    ssize_t input_length;
} buffer_t;

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

typedef enum
{
    META_COMMAND_SUCCESS,
    META_COMMAND_UNRECOGNIZED_COMMAND
} meta_command_result_t;

typedef enum
{
    PREPARE_SUCCESS,
    PREPARE_UNRECOGNIZED_STATEMENT
} prepare_result_t;

// 执行 meta 命令: .exit, etc
meta_command_result_t do_meta_command(buffer_t *input)
{
    if (strcmp(input->buffer, ".exit") == 0)
    {
        exit(EXIT_SUCCESS);
    }
    else
    {
        return META_COMMAND_UNRECOGNIZED_COMMAND;
    }
}

typedef enum
{
    STATEMENT_INSERT,
    STATEMENT_SELECT
} statement_type_t;

typedef struct
{
    statement_type_t type;
} statement_t;

// 执行 insert, select 命令
prepare_result_t prepare_statement(buffer_t *input, statement_t *statement)
{
    if (strncmp(input->buffer, "insert", 6) == 0)
    {
        statement->type = STATEMENT_INSERT;
        return PREPARE_SUCCESS;
    }
    if (strncmp(input->buffer, "select", 6) == 0)
    {
        statement->type = STATEMENT_SELECT;
        return PREPARE_SUCCESS;
    }
    return PREPARE_UNRECOGNIZED_STATEMENT;
}

void execute_statement(statement_t *statement)
{
    switch (statement->type)
    {
    case STATEMENT_INSERT:
        printf("insert action here. \n");
        break;
    case STATEMENT_SELECT:
        printf("select action here. \n");
        break;
    default:
        break;
    }
}

#endif