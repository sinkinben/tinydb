#ifdef OPEN_TRANSACTION
#include "list.h"
#include "types.h"
#ifndef TRANSACTION_H
#define TRANSACTION_H
typedef struct
{
    cursor_t cursor;
    statement_t statement;
    list_node_t entry;
} transaction_t;
typedef transaction_t trans_t;

// a dummy list node
trans_t *trans = NULL;

// this function will run before main
void __attribute__((constructor)) init_transaction()
{
    trans = (trans_t *)malloc(sizeof(trans_t));
    init_list_head(&(trans->entry));
}

void __attribute__((destructor)) free_transaction()
{
    list_node_t *pos, *next;
    list_for_each_safe(pos, next, &(trans->entry))
    {
        if (pos != NULL)
        {
            list_del(pos);
            free(list_entry(pos, trans_t, entry));
        }
    }
    if (trans != NULL)
        free(trans);
}

void transaction_push(cursor_t *cursor, statement_t *statement)
{
    trans_t *node = (trans_t *)malloc(sizeof(trans_t));
    memcpy((void *)&(node->cursor), (const void *)cursor, sizeof(cursor_t));
    memcpy((void *)&(node->statement), (const void *)statement, sizeof(statement_t));
    list_add_tail(&(node->entry), &(trans->entry));
}

trans_t *transaction_pop()
{
    list_node_t *last = (trans->entry).prev;
    list_del(last);
    return list_entry(last, trans_t, entry);
}

void transaction_commit()
{
    // TODO: 有可能是写同一页的, 需要优化一下
    // 在 pager_t 中加入 dirty page 标记
    list_node_t *pos, *next;
    list_for_each_safe(pos, next, &(trans->entry))
    {
        assert(pos != NULL);

        trans_t *node = list_entry(pos, trans_t, entry);
        cursor_t *cursor = &(node->cursor);
        pager_t *pager = cursor->table->pager;

        assert(cursor->page_num < pager->num_pages);

        pager_flush(pager, cursor->page_num);
        fsync(pager->file_descriptor);
        list_del(pos), free(node);
    }
}

// rollback only one step
void transaction_rollback()
{
    trans_t *last = transaction_pop();
    cursor_t *cursor = &(last->cursor);
    statement_t *statement = &(last->statement);
    row_t *row = &(statement->row_value);
    switch (statement->type)
    {
    case STATEMENT_INSERT:
        leaf_node_fake_delete(cursor, row->id);
        break;
    case STATEMENT_DELETE:
        leaf_node_insert(cursor, row->id, row);
        break;
    case STATEMENT_UPDATE:
        serialize_row(row, cursor_value(cursor));
        break;
    default:
        assert(0);
    }
    free(last);
}
#endif
#endif