#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#ifndef TABLE_H
#define TABLE_H

#define COLUMN_USERNAME_SIZE 32
#define COLUMN_EMAIL_SIZE 255
typedef struct
{
    uint32_t id;
    char username[COLUMN_USERNAME_SIZE];
    char email[COLUMN_EMAIL_SIZE];
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

void serialize_row(row_t *source, void *dest)
{
    memcpy(dest + ID_OFFSET, &(source->id), ID_SIZE);
    memcpy(dest + USERNAME_OFFSET, &(source->username), USERNAME_SIZE);
    memcpy(dest + EMAIL_OFFSET, &(source->email), EMAIL_SIZE);
}

void deserialize_row(void *source, row_t *dest)
{
    memcpy(&(dest->id), source + ID_OFFSET, ID_SIZE);
    memcpy(&(dest->username), source + USERNAME_OFFSET, USERNAME_SIZE);
    memcpy(&(dest->email), source + EMAIL_OFFSET, EMAIL_SIZE);
}

const uint32_t TABLE_MAX_PAGES = 100;

const uint32_t PAGE_SIZE = 4096; // 4KB
const uint32_t ROWS_PER_PAGE = PAGE_SIZE / ROW_SIZE;
const uint32_t TABLE_MAX_ROWS = TABLE_MAX_PAGES * ROWS_PER_PAGE;

// 暂且不用 B+ 树, 使用数组的形式作为存储结构
typedef struct
{
    uint32_t num_rows;
    void *pages[TABLE_MAX_PAGES];
} table_t;

void *get_row_slot(table_t *table, uint32_t row_num)
{
    uint32_t page_num = row_num / ROWS_PER_PAGE;
    void *page = table->pages[page_num];
    if (page == NULL)
    {
        page = table->pages[page_num] = malloc(PAGE_SIZE);
    }
    uint32_t row_offset = row_num % ROWS_PER_PAGE;
    uint32_t bytes_offset = row_offset * ROW_SIZE;
    return page + bytes_offset;
}

void print_row(row_t *row)
{
    printf("(%d %s %s)\n", row->id, row->username, row->email);
}

table_t *new_table()
{
    table_t *table = (table_t *)malloc(sizeof(table_t));
    table->num_rows = 0;
    for (uint32_t i = 0; i < TABLE_MAX_PAGES; i++)
    {
        table->pages[i] = NULL;
    }
    return table;
}

void free_table(table_t *table)
{
    for (uint32_t i = 0; i < TABLE_MAX_PAGES; i++)
    {
        if (table->pages[i] != NULL)
            free(table->pages[i]);
    }
    free(table);
}

#endif