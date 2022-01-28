#include "sql.h"
#ifndef __CONDITION_H__
#define __CONDITION_H__
typedef enum
{
    OP_EQUAL,
    OP_GREAT,
    OP_GREAT_EQUAL,
    OP_LESS,
    OP_LESS_EQUAL,
    OP_NOT_EQUAL,
    OP_AND,
    OP_OR
} operator_t;

/**
 * A condition_t denote: `lvalue op rvalue`,
 * 1) `lvalue` is always the column name.
 * 2) `rvalue` could be a integer or a string.
 * 3) When is_leaf = 1, then we should use lvalue/rvalue.
 * 4) When is_leaf = 0, then we should use lchild/rchild.
 */
struct condition_t
{
    operator_t op;
    bool is_leaf;
    union
    {
        struct condition_t *lchild;
        schema_t *lvalue;
    };
    union
    {
        struct condition_t *rchild;
        union
        {
            uint64_t intval;
            char *strval;
        } rvalue;
    };
};
typedef struct condition_t condition_t;

static condition_t *alloc_condition(uint64_t lchild, operator_t op, uint64_t rchild, bool is_leaf)
{
    condition_t *cond = (condition_t *)malloc(sizeof(condition_t));
    cond->op = op;
    cond->is_leaf = is_leaf;
    cond->lchild = (condition_t *)(lchild);
    cond->rchild = (condition_t *)(rchild);
    return cond;
}

static void print_tree(condition_t *node, int depth)
{
    if (node == NULL)
        return;
    for (int i = 0; i < depth; ++i)
        printf("\t- ");
    if (node->is_leaf)
    {
        printf("lvalue = %s, op = %d, rvalue = %p \n", node->lvalue->fieldname, node->op, node->rchild);
    }
    else
    {
        printf("op = %d\n", node->op);
        print_tree(node->lchild, depth + 1);
        print_tree(node->rchild, depth + 1);
    }
}


#endif