#include <stdbool.h>
#ifndef TYPES_H
#define TYPES_H

typedef struct
{
    char *buffer;
    size_t buffer_length;
    ssize_t input_length;
} buffer_t;

typedef enum
{
    META_COMMAND_SUCCESS,
    META_COMMAND_UNRECOGNIZED_COMMAND
} meta_command_result_t;

typedef enum
{
    PREPARE_SUCCESS,
    PREPARE_SYNTAX_ERROR,
    PREPARE_UNRECOGNIZED_STATEMENT
} prepare_result_t;

typedef enum
{
    STATEMENT_INSERT,
    STATEMENT_SELECT
} statement_type_t;

// insert, select 等命令的解析结果
// 相当于一个简易的 sql-parser
typedef struct
{
    statement_type_t type;
    row_t row_to_insert;
} statement_t;

typedef enum
{
    EXECUTE_SUCCESS,
    EXECUTE_UNKNOWN_STATEMENT,
    EXECUTE_TABLE_FULL
} execute_result_t;

#endif