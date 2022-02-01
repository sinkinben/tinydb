#include "list.h"
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#ifndef __SCHEMA_H__
#define __SCHEMA_H__

static const uint32_t TBALE_COLUMN_NAME_MAX_LENGTH = 32;
typedef enum
{
    DUMMY = -1,
    COLUMN_INT,    // -> uint_t in tinydb kernel
    COLUMN_VARCHAR // -> varchar(n) in tinydb kernel
} column_type_t;

/* Table Schema
 * This struct has two usages:
 * 1) denote a column when executing create-sql
 * 2) denote a table name temporarily when `column_type` = -1
 */
typedef struct
{
    column_type_t column_type;
    uint32_t width;
    char fieldname[TBALE_COLUMN_NAME_MAX_LENGTH];
} schema_t;

struct schema_node_t
{
    struct list_head entry;
    schema_t schema;
};
typedef struct schema_node_t schema_node_t;


static inline schema_node_t *alloc_schema_node(const char *filedname, uint32_t width, column_type_t column_type)
{
    schema_node_t *node = (schema_node_t *)malloc(sizeof(schema_node_t));
    strcpy(node->schema.fieldname, filedname);
    node->schema.width = width;
    node->schema.column_type = column_type;
    init_list_head(&(node->entry));
    return node;
}

static inline void free_schema_list(schema_node_t *schemas)
{
    if (list_empty(&(schemas->entry)))
        return;
    list_node_t *pos, *next;
    list_for_each_safe(pos, next, &schemas->entry)
    {
        list_del(pos);
        free(list_entry(pos, schema_node_t, entry));
    }
    init_list_head(&(schemas->entry));
}


#endif