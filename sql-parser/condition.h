#include <assert.h>
#include "schema.h"
#include "types.h"
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
 * A condition_t struct denotes: `lvalue op rvalue`,
 * 1) `lvalue` is always the column name.
 * 2) `rvalue` could be a integer or a string, which is denoted by `rvalue_type`.
 * 3) When is_leaf = 1, then we should use lvalue/rvalue.
 * 4) When is_leaf = 0, then we should use lchild/rchild.
 */
struct condition_t
{
    operator_t op;
    bool is_leaf;
    column_type_t cmp_type;
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

static inline condition_t *alloc_condition(
    uint64_t lchild,
    operator_t op,
    uint64_t rchild,
    bool is_leaf,
    column_type_t cmp_type
)
{
    condition_t *cond = (condition_t *)malloc(sizeof(condition_t));
    cond->op = op;
    cond->is_leaf = is_leaf;
    cond->lchild = (condition_t *)(lchild);
    cond->rchild = (condition_t *)(rchild);
    cond->cmp_type = cmp_type;
    return cond;
}

static inline void print_condition_tree(condition_t *node, int depth)
{
    if (node == NULL)
        return;
    for (int i = 0; i < depth; ++i)
        printf("\t- ");
    if (node->is_leaf)
    {
        printf("lvalue = %s, op = %d, rvalue = %p, cmp_type = %d \n",
                node->lvalue->fieldname, node->op, node->rchild, node->cmp_type);
    }
    else
    {
        printf("op = %d\n", node->op);
        print_condition_tree(node->lchild, depth + 1);
        print_condition_tree(node->rchild, depth + 1);
    }
}

static inline bool cmp_int(operator_t op, uint64_t lval, uint64_t rval)
{
    switch (op)
    {
    case OP_EQUAL:
        return lval == rval;
    case OP_GREAT:
        return lval > rval;
    case OP_GREAT_EQUAL:
        return lval >= rval;
    case OP_LESS:
        return lval < rval;
    case OP_LESS_EQUAL:
        return lval <= rval;
    case OP_NOT_EQUAL:
        return lval != rval;
    default:
        printf("[cmp_int] Unknow op %d. \n", op);
        assert(0);
    }
}

static inline bool cmp_string(operator_t op, const char *lval, const char *rval)
{
    int ret = strcmp(lval, rval);
    switch (op)
    {
    case OP_EQUAL:
        return ret == 0;
    case OP_GREAT:
        return ret == 1;
    case OP_GREAT_EQUAL:
        return ret >= 0;
    case OP_LESS:
        return ret == -1;
    case OP_LESS_EQUAL:
        return ret <= 0;
    case OP_NOT_EQUAL:
        return ret != 0;
    default:
        break;
    }
    printf("[cmp_string] Unknow op %d. \n", op);
    assert(0);
    return -1;
}

static inline bool test_operator(column_type_t type, operator_t op, uint64_t lval, uint64_t rval)
{
    if (type == COLUMN_INT)
        return cmp_int(op, lval, rval);
    else if (type == COLUMN_VARCHAR)
        return cmp_string(op, (const char *)(lval), (const char *)(rval));
    else
    {
        switch (op)
        {
        case OP_AND:
            return lval && rval;
        case OP_OR:
            return lval || rval;
        default:
            break;
        }
        printf("[test_operator] Unknow column type %d and op %d. \n", type, op);
        assert(0);
        return 0;
    }
}

static inline uint64_t get_row_field(const char *field, row_t *row)
{
    if (strcmp("id", field) == 0)
        return (uint64_t)(row->id);
    else if (strcmp("username", field) == 0)
        return (uint64_t)(row->username);
    else if (strcmp("email", field) == 0)
        return (uint64_t)(row->email);
    else
    {
        printf("[get_row_field] Unknow field \'%s\' in condition. \n", field);
        assert(0);
        return -1;
    }
}

static inline bool test_condition(condition_t *cond, row_t *row)
{
    if (cond == NULL)
        return false;
    if (!cond->is_leaf)
    {
        uint64_t lval = test_condition(cond->lchild, row);
        uint64_t rval = test_condition(cond->rchild, row);
        return test_operator(cond->cmp_type, cond->op, lval, rval);
    }
    else
    {
        uint64_t lval = get_row_field(cond->lvalue->fieldname, row);
        uint64_t rval = cond->rvalue.intval;
        return test_operator(cond->cmp_type, cond->op, lval, rval);
    }
}

static inline void destroy_condition_tree(condition_t *node)
{
    if (node == NULL)
        return;
    if (!(node->is_leaf))
    {
        destroy_condition_tree(node->lchild);
        destroy_condition_tree(node->rchild);
    }
    else
    {
        void *entry_ptr = (void *)(node->lvalue) - sizeof(list_node_t);
        free(entry_ptr);
        if (node->cmp_type == COLUMN_VARCHAR)
            free((void *)(node->rvalue.strval));
        free((void *)(node));
    }
}


#endif