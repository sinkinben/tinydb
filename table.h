#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "types.h"
#include "pager.h"
#include "cursor.h"
#include "btree.h"
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
    printf("(%d, %s, %s)\n", row->id, row->username, row->email);
}

// opening the database file
// initializing a pager data structure
// initializing a table data structure
table_t *db_open(const char *filename)
{
    pager_t *pager = pager_open(filename);
    table_t *table = (table_t *)malloc(sizeof(table_t));
    table->pager = pager;
    table->root_page_num = 0;
    if (pager->num_pages == 0)
    {
        // New database file. Initialize page 0 as leaf node.
        void *root_node = get_page(pager, 0);
        init_leaf_node(root_node);
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

#endif