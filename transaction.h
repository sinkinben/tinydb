#include "list.h"
#ifndef TRANSACTION_H
#define TRANSACTION_H
typedef struct
{
    statement_t statement;
    list_node_t entry;
} transaction_t;
typedef transaction_t trans_t;

// a dummy list node
static trans_t *trans = NULL;

// this function will run before main
static inline void __attribute__((constructor)) init_transaction()
{
    trans = (trans_t *)malloc(sizeof(trans_t));
    init_list_head(&(trans->entry));
}

static inline void __attribute__((destructor)) free_transaction()
{
    list_node_t *pos, *next;
    list_for_each_safe(pos, next, &(trans->entry))
    {
        if (pos != NULL)
            free(pos);
    }
    if (trans != NULL)
        free(trans);
}

static inline void transaction_push(statement_t *statement)
{
    trans_t *node = (trans_t *)malloc(sizeof(trans_t));
    memcpy(&(node->statement), statement, sizeof(statement_t));
    list_add_tail(&(node->entry), &(trans->entry));
}

static inline trans_t *transaction_pop()
{
    list_node_t *last = (trans->entry).prev;
    list_del(last);
    return list_entry(last, trans_t, entry);
}

static inline void transaction_commit()
{
    list_node_t *pos, *next;
    trans_t *node;
    list_for_each_safe(pos, next, &(trans->entry))
    {
        node = list_entry(pos, trans_t, entry);
        // TODO: commit, write page to disk: memory -> disk
    }
}

// rollback only one step
static inline void transaction_rollback()
{
    trans_t *last = transaction_pop();
    // TODO: rollback, read page from disk: memory <- disk
}

#endif