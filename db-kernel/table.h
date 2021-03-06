#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "types.h"
#include "pager.h"
#include "btree.h"
#include "sql-parser/schema.h"
#ifndef TABLE_H
#define TABLE_H

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

void print_row(row_t *row)
{
    printf("(%u, %s, %s)\n", row->id, row->username, row->email);
}

uint8_t get_field_flags(schema_node_t *schemas)
{
    if (schemas == NULL)
        return 7;

    uint8_t flags = 0;
    list_node_t *pos;
    list_for_each(pos, &(schemas->entry))
    {
        const char *field = list_entry(pos, schema_node_t, entry)->schema.fieldname;
        if (strcmp(field, "id") == 0)
            flags |= 1;
        else if (strcmp(field, "username") == 0)
            flags |= 2;
        else if (strcmp(field, "email") == 0)
            flags |= 4;
    }
    if (flags == 0) flags = 7;
    return flags;
}

void print_fields(row_t *row, uint8_t flags)
{
    if (flags & 1)
        printf("id = %u ", row->id);

    if (flags & 2)
        printf("username = %s ", row->username);

    if (flags & 4)
        printf("email = %s ", row->email);

    printf("\n");
}

// opening the database file
// initializing a pager data structure
// initializing a table data structure
table_t *db_open(const char *filename)
{
    pager_t *pager = pager_open(filename);
    table_t *table = (table_t *)malloc(sizeof(table_t));
    table->table_name = filename;
    table->pager = pager;
    table->root_page_num = 0;
    if (pager->num_pages == 0)
    {
        // New database file. Initialize page 0 as leaf node.
        void *root_node = get_page(pager, 0);
        init_leaf_node(root_node);
        set_node_root(root_node, true);
    }
    return table;
}

// flushes the page cache to disk
// closes the database file
// frees the memory for the Pager and Table data structures
void db_close(table_t *table)
{
    pager_t *pager = table->pager;

    for (uint32_t i = 0; i < pager->num_pages; i++)
    {
        if (pager->pages[i] == NULL)
            continue;
        pager_flush(pager, i);
        free(pager->pages[i]);
        pager->pages[i] = NULL;
    }

    // There may be a partial page to write to the end of the file
    // This should not be needed after we switch to a B-tree
    // uint32_t num_additional_rows = table->num_rows % ROWS_PER_PAGE;
    // if (num_additional_rows > 0)
    // {
    //     uint32_t page_num = num_full_pages;
    //     if (pager->pages[page_num] != NULL)
    //     {
    //         pager_flush(pager, page_num, num_additional_rows * ROW_SIZE);
    //         free(pager->pages[page_num]);
    //         pager->pages[page_num] = NULL;
    //     }
    // }

    int ret = close(pager->file_descriptor);
    if (ret < 0)
    {
        printf("Error closing db file.\n");
        exit(EXIT_FAILURE);
    }

    for (uint32_t i = 0; i < TABLE_MAX_PAGES; i++)
    {
        if (pager->pages[i] != NULL)
        {
            free(pager->pages[i]);
            pager->pages[i] = NULL;
        }
    }
    free(pager);
    free(table);
}

/**
 * Return the position of the given key.
 * If the key is not present, return the position
 * where it should be inserted
 **/
cursor_t *table_find(table_t *table, uint32_t key)
{
    uint32_t root_page_num = table->root_page_num;
    void *root_node = get_page(table->pager, root_page_num);

    if (get_node_type(root_node) == NODE_LEAF)
    {
        return leaf_node_find(table, root_page_num, key);
    }
    else
    {
        // printf("Need to implement searching an internal node\n");
        // exit(EXIT_FAILURE);
        return internal_node_find(table, root_page_num, key);
    }
}

/**
 * Check whether the key exists in the table,
 * If not, return NULL. Otherwise, return cursor that points to the row in table
 * Consider if `key_to_find` > max_key in leaf node:
 *   - each cell is (uint32_t key, row_t value), total 294 bytes
 *   - in leaf node(page), we have `n = num_cells` cells (index from 0, ..., n-1)
 *   - in such case, cursor->cell_num will >= num_cells
 **/
cursor_t *table_exists(table_t *table, uint32_t key_to_find)
{
    cursor_t *cursor = table_find(table, key_to_find);
    void *node = get_page(cursor->table->pager, cursor->page_num);
    // printf("[table exists] key = %d\n", key_to_find);
    // printf("[table exists] page = %d\n", cursor->page_num);
    // printf("[table exists] cell = %d\n", cursor->cell_num);
    // printf("[table exists] num_cells = %d\n", *leaf_node_num_cells(node));

    // In this case, the row address `row = cursor_value(cursor)` is invalid,
    // we should not access memory
    // even if we fill zeros when initialize the leaf node
    if (cursor->cell_num >= *leaf_node_num_cells(node))
    {
        free(cursor);
        return NULL;
    }

    void *row = cursor_value(cursor);
    row_t temp;
    deserialize_row(row, &temp);
    if (temp.id != key_to_find)
    {
        free(cursor);
        cursor = NULL;
    }
    return cursor;
}

#endif