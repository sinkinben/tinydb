#include "list.h"
#include <stdlib.h>
typedef struct
{
    int value;
    list_node_t entry;
} node_t;
int main()
{
    int i = 0;
    node_t *p;
    list_node_t *pos;

    // allocate a dummy head node
    node_t *head = (node_t *)malloc(sizeof(node_t));
    head->value = -1;
    init_list_head(&head->entry);

    // add nodes
    for (i = 0; i < 10; i++)
    {
        p = (node_t *)malloc(sizeof(node_t));
        p->value = i + 1;
        list_add_tail(&p->entry, &head->entry);
    }

    // traversal
    list_for_each(pos, &head->entry)
    {
        printf("%d, ", list_entry(pos, node_t, entry)->value);
    }

    // free nodes
    list_node_t *next;
    list_for_each_safe(pos, next, &head->entry)
    {
        list_del(pos);
        free(list_entry(pos, node_t, entry));
    }

    // free head
    free(head);
}