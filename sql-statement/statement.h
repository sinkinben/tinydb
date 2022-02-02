#include "sql-parser/condition.h"
#include "types.h"
#ifndef STATEMENT_H
#define STATEMENT_H

typedef enum
{
    STATEMENT_INSERT,
    STATEMENT_SELECT,
    STATEMENT_SELECT_WHERE,
    STATEMENT_UPDATE,
    STATEMENT_UPDATE_WHERE,
    STATEMENT_DELETE,
    STATEMENT_DELETE_WHERE,
    STATEMENT_COMMIT,
    STATEMENT_ROLLBACK
} statement_type_t;


typedef struct
{
    statement_type_t type;   // SELECT, UPDATE, DELETE, etc.
    row_t row_value;         // The row value of INSERT SQL
    const char *table_name;  // table name
    schema_node_t *schemas;  // SELECT/UPDATE column items list
    condition_t *conditions; // WHERE conditions
} statement_t;

static inline void statement_set(
    statement_t *stm,
    statement_type_t type,
    const char *table_name,
    schema_node_t *schemas,
    condition_t *conds
)
{
    stm->type = type;
    stm->table_name = table_name;
    stm->schemas = schemas;
    stm->conditions = conds;
    memset(&(stm->row_value), 0, sizeof(row_t));
}

static inline void statement_init(statement_t *stm)
{
    statement_set(stm, -1, NULL, NULL, NULL);
}

/* Free the resource that were allocated in sql parser,
 * and keep the `stm` struct.
 */
static inline void statement_free(statement_t *stm)
{
    assert(stm != NULL);
    if (stm->schemas)
    {
        free_schema_list(stm->schemas);
        stm->schemas = NULL;
    }
    if (stm->table_name)
    {
        free((void *)(stm->table_name));
        stm->table_name = NULL;
    }
    if (stm->conditions)
    {
        destroy_condition_tree(stm->conditions);
        stm->conditions = NULL;
    }
}

#endif