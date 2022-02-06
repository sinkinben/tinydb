
#include <stdbool.h>
#include <stdio.h>
#ifndef TINYDB_LIST_H
#define TINYDB_LIST_H
struct list_head
{
    struct list_head *next, *prev;
};
typedef struct list_head list_node_t;

#define LIST_HEAD_INITIALIZER(name) \
    {                               \
        .next = &(name),            \
        .prev = &(name)             \
    }

#define LIST_HEAD(name) \
    list_node_t name = LIST_HEAD_INITIALIZER(name)

#define list_for_each(pos, head) \
    for (pos = (head)->next; pos != (head); pos = pos->next)

#define list_for_each_prev(pos, head) \
    for (pos = (head)->prev; pos != (head); pos = pos->prev)

// use a variable `next` to save the next node of `pos`
// Why we need this API?
// In some case, we need to traversal, but delete the node at the same time.
#define list_for_each_safe(pos, next, head)    \
    for (pos = (head)->next, next = pos->next; \
         pos != (head);                        \
         pos = next, next = pos->next)

#define offsetof(type, member) ((uint32_t) & (((type *)0)->member))

#define container_of(ptr, type, member) (                  \
    {                                                      \
        const typeof(((type *)0)->member) *__mptr = (ptr); \
        (type *)((char *)__mptr - offsetof(type, member)); \
    })

#define list_entry(ptr, type, member) container_of(ptr, type, member)

static inline void init_list_head(list_node_t *entry)
{
    entry->next = entry->prev = entry;
}

// the head is a dummy node
static inline bool list_empty(const list_node_t *head)
{
    return head->next == head;
}

/**
 * - https://www.kernel.org/doc/htmldocs/kernel-api/API-list-empty-careful.html
 * - 同时判断头指针的 next 和 prev, 仅当两者都指向自己时才返回真
 * - 这主要是为了应付另一个 CPU 正在处理同一个链表而造成 next, prev 不一致的情况
 * - 这一安全保障能力有限：除非其他 CPU 的链表操作只有 list_del_init(), 否则仍然不能保证安全
 * - 还是得加锁机制
 **/
static inline bool list_empty_careful(const list_node_t *head)
{
    list_node_t *next = head->next;
    return (next == head) && (next == head->prev);
}

// insert `new` node into `prev -> (new) -> next`
static inline void __list_add(list_node_t *new, list_node_t *prev, list_node_t *next)
{
    next->prev = new, new->next = next;
    new->prev = prev, prev->next = new;
}

// delete `node` between `prev -> (node) -> next`
static inline void __list_del(list_node_t *prev, list_node_t *next)
{
    next->prev = prev;
    prev->next = next;
}

static inline void list_add(list_node_t *new, list_node_t *head)
{
    __list_add(new, head, head->next);
}

static inline void list_add_tail(list_node_t *new, list_node_t *head)
{
    __list_add(new, head->prev, head);
}

static inline void list_del(list_node_t *entry)
{
    __list_del(entry->prev, entry->next);
    entry->next = entry->prev = NULL;
    // we do not delete entry here
}

static inline void list_del_init(list_node_t *entry)
{
    __list_del(entry->prev, entry->next);
    init_list_head(entry);
}

static inline void list_move(list_node_t *node, list_node_t *head)
{
    // delete `node`
    __list_del(node->prev, node->next);
    // add `node` after dummy head
    list_add(node, head);
}

static inline void list_move_tail(list_node_t *node, list_node_t *head)
{
    // delete `node`
    __list_del(node->prev, node->next);
    // add `node` to the tail of the list that `head` points to
    list_add_tail(node, head);
}

// TODO: list_splice

#endif