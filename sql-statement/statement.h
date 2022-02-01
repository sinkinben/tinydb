#include "sql-parser/condition.h"
#include "types.h"
#ifndef STATEMENT_H
#define STATEMENT_H

typedef enum
{
    STATEMENT_INSERT,
    STATEMENT_SELECT,
    STATEMENT_UPDATE,
    STATEMENT_DELETE,
    STATEMENT_COMMIT,
    STATEMENT_ROLLBACK
} statement_type_t;


typedef struct
{
    statement_type_t type;   /* SELECT, UPDATE, DELETE, etc. */
    row_t row_value;         /* The row value of INSERT SQL  */
    const char *table_name;  /* table name                   */
    schema_node_t *schemas;  /* SELECT column items list     */
    condition_t *conditions; /* WHERE conditions             */
} statement_t;

static inline void statement_init(statement_t *stm)
{
    stm->type = -1;
    stm->table_name = NULL;
    stm->schemas = NULL;
    stm->conditions = NULL;
    memset(&(stm->row_value), 0, sizeof(row_t));
}

static inline void statement_free(statement_t *stm)
{
    // TODO
    assert(0);
}

#endif