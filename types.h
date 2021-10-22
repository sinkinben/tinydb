#include <stdint.h>
#include <stdio.h>
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
    PARSE_SUCCESS,
    PARSE_SYNTAX_ERROR,
    PARSE_STRING_TOO_LONG,
    PARSE_UNIMPLEMENTED,
    PARSE_UNRECOGNIZED_STATEMENT
} parse_result_t;

const char *STATEMENT_KEYWORDS[] = {"insert", "select", "update", "delete", "commit", "rollback"};
typedef enum
{
    STATEMENT_INSERT,
    STATEMENT_SELECT,
    STATEMENT_UPDATE,
    STATEMENT_DELETE,
    STATEMENT_COMMIT,
    STATEMENT_ROLLBACK
} statement_type_t;
#define statement_keyword(statement_type) STATEMENT_KEYWORDS[(statement_type)]

// table 的一行·
#define COLUMN_USERNAME_SIZE 32
#define COLUMN_EMAIL_SIZE 255
typedef struct
{
    uint32_t id;
    char username[COLUMN_USERNAME_SIZE + 1];
    char email[COLUMN_EMAIL_SIZE + 1];
} row_t;

#define size_of_attribute(Struct, Attribute) sizeof(((Struct *)0)->Attribute)
const uint32_t ID_SIZE = size_of_attribute(row_t, id);
const uint32_t USERNAME_SIZE = size_of_attribute(row_t, username);
const uint32_t EMAIL_SIZE = size_of_attribute(row_t, email);
const uint32_t ROW_SIZE = ID_SIZE + USERNAME_SIZE + EMAIL_SIZE;

#define offset_of_attribute(Struct, Attribute) ((uint32_t)(&((Struct *)0)->Attribute))
const uint32_t ID_OFFSET = offset_of_attribute(row_t, id);
const uint32_t USERNAME_OFFSET = offset_of_attribute(row_t, username);
const uint32_t EMAIL_OFFSET = offset_of_attribute(row_t, email);

// insert, select 等命令的解析结果
// 相当于一个简易的 sql-parser
typedef struct
{
    statement_type_t type;
    row_t row_value;
} statement_t;

typedef enum
{
    EXECUTE_SUCCESS,
    EXECUTE_DUPLICATE_KEY,
    EXECUTE_NO_SUCH_KEY,
    EXECUTE_UNKNOWN_STATEMENT,
    EXECUTE_TABLE_FULL
} execute_result_t;

const uint32_t TABLE_MAX_PAGES = 1 << 20;

const uint32_t PAGE_SIZE = 4096; // 4KB
// const uint32_t ROWS_PER_PAGE = PAGE_SIZE / ROW_SIZE;
// const uint32_t TABLE_MAX_ROWS = TABLE_MAX_PAGES * ROWS_PER_PAGE;

typedef struct
{
    int file_descriptor;
    uint32_t file_length;
    uint32_t num_pages;
    void *pages[TABLE_MAX_PAGES]; // 每个 pages[i] 映射到 disk 上的一个 page
} pager_t;

// 暂且不用 B+ 树, 使用数组的形式作为存储结构
typedef struct
{
    uint32_t root_page_num;
    pager_t *pager;
} table_t;

typedef struct
{
    table_t *table;
    uint32_t page_num; // cursor 当前指向哪一 page
    uint32_t cell_num; // cursor 当前指向 page 中的哪一个 cell
    bool end_of_table; // this indicates a position one past the last element
} cursor_t;

#endif