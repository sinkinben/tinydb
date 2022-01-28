#include "../list.h"
#include <stdint.h>
#include <stdlib.h>
#ifndef __SQL_H__
#define __SQL_H__

const uint32_t TBALE_COLUMN_NAME_MAX_LENGTH = 32;
typedef enum
{
    COLUMN_INT,    // -> uint_t in tinydb kernel
    COLUMN_VARCHAR // -> varchar(n) in tinydb kernel
} column_type_t;

/* Table Schema */
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


#endif