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


// table 的一行
#define COLUMN_USERNAME_SIZE 32
#define COLUMN_EMAIL_SIZE 255
typedef struct
{
    uint32_t id;
    char username[COLUMN_USERNAME_SIZE + 1];
    char email[COLUMN_EMAIL_SIZE + 1];
} row_t;

#define size_of_attribute(Struct, Attribute) sizeof(((Struct *)0)->Attribute)
static const uint32_t ID_SIZE = size_of_attribute(row_t, id);
static const uint32_t USERNAME_SIZE = size_of_attribute(row_t, username);
static const uint32_t EMAIL_SIZE = size_of_attribute(row_t, email);
static const uint32_t ROW_SIZE = ID_SIZE + USERNAME_SIZE + EMAIL_SIZE;

#define offset_of_attribute(Struct, Attribute) ((uint32_t)(&((Struct *)0)->Attribute))
static const uint32_t ID_OFFSET = offset_of_attribute(row_t, id);
static const uint32_t USERNAME_OFFSET = offset_of_attribute(row_t, username);
static const uint32_t EMAIL_OFFSET = offset_of_attribute(row_t, email);


typedef enum
{
    EXECUTE_SUCCESS,
    EXECUTE_DUPLICATE_KEY,
    EXECUTE_NO_SUCH_KEY,
    EXECUTE_UNKNOWN_STATEMENT,
    EXECUTE_TABLE_FULL
} execute_result_t;

static const uint32_t TABLE_MAX_PAGES = 1 << 20;

static const uint32_t PAGE_SIZE = 4096; // 4KB
// const uint32_t ROWS_PER_PAGE = PAGE_SIZE / ROW_SIZE;
// const uint32_t TABLE_MAX_ROWS = TABLE_MAX_PAGES * ROWS_PER_PAGE;

typedef struct
{
    int file_descriptor;
    uint32_t file_length;
    uint32_t num_pages;
    void *pages[TABLE_MAX_PAGES]; // 每个 pages[i] 映射到 disk 上的一个 page
} pager_t;

typedef struct
{
    uint32_t root_page_num;
    pager_t *pager;
    const char *table_name;
} table_t;

typedef struct
{
    table_t *table;
    uint32_t page_num; // cursor 当前指向哪一 page
    uint32_t cell_num; // cursor 当前指向 page 中的哪一个 cell
    bool end_of_table; // this indicates a position one past the last element
} cursor_t;

#endif