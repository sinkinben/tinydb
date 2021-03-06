#include "btree.h"
#ifndef DEBUG_H
#define DEBUG_H
void print_max_cells()
{
    printf("INTERNAL_NODE_MAX_CELLS = %u\n", INTERNAL_NODE_MAX_CELLS);
    printf("LEAF_NODE_MAX_CELLS = %u\n", LEAF_NODE_MAX_CELLS);
}
void print_constants()
{
    printf("table row size: %u\n", ROW_SIZE);
    printf("  - id:       %u\n", ID_SIZE);
    printf("  - username: %u\n", USERNAME_SIZE);
    printf("  - email:    %u\n", EMAIL_SIZE);

    printf("common node header size: %d\n", COMMON_NODE_HEADER_SIZE);
    printf("  - node_type: %u\n", NODE_TYPE_SIZE);
    printf("  - is_root:   %u\n", IS_ROOT_SIZE);
    printf("  - parent:    %u\n", PARENT_POINTER_SIZE);

    printf("leaf node:\n");
    printf("  + header size:      %u\n", LEAF_NODE_HEADER_SIZE);
    printf("    - num_cells: %u\n", LEAF_NODE_NUM_CELLS_SIZE);
    printf("    - next_leaf: %u\n", LEAF_NODE_NEXT_LEAF_SIZE);
    printf("  + cell size:        %u\n", LEAF_NODE_CELL_SIZE);
    printf("    - key size: %u\n", LEAF_NODE_KEY_SIZE);
    printf("    - row size: %u\n", LEAF_NODE_VALUE_SIZE);
    printf("  + spaces for cells: %u\n", LEAF_NODE_SPACE_FOR_CELLS);
    // printf("  + max cells:        %d\n", LEAF_NODE_MAX_CELLS);

    printf("internal node:\n");
    printf("  + header size:      %u\n", INTERNAL_NODE_HEADER_SIZE);
    printf("    - num_cells:   %u\n", INTERNAL_NODE_NUM_KEYS_SIZE);
    printf("    - right_child: %u\n", INTERNAL_NODE_RIGHT_CHILD_SIZE);
    printf("  + cell size:        %u\n", INTERNAL_NODE_CELL_SIZE);
    printf("    - child: %u\n", INTERNAL_NODE_CHILD_SIZE);
    printf("    - key:   %u\n", INTERNAL_NODE_KEY_SIZE);
    printf("  + spaces for cells: %u\n", INTERNAL_NODE_SPACE_FOR_CELLS);
    // printf("  + max cells:        %d\n", INTERNAL_NODE_MAX_CELLS);
}

void indent(int32_t level)
{
    for (int32_t i = 0; i < level; i++)
    {
        printf("  ");
    }
}

void print_btree(pager_t *pager, uint32_t page_num, uint32_t indent_level)
{
    void *node = get_page(pager, page_num);
    // child is actually page num of a disk file
    uint32_t num_keys, child;
    node_type_t node_type = get_node_type(node);
    if (node_type == NODE_INTERNAL)
    {
        num_keys = *internal_node_num_keys(node);
        indent(indent_level);
        printf("- internal (size %u, page %u, parent %u)\n", num_keys, page_num, *node_parent(node));
        // printf("- internal (size %d)\n", num_keys);
        for (uint32_t i = 0; i < num_keys; i++)
        {
            child = *internal_node_child(node, i);
            print_btree(pager, child, indent_level + 1);

            indent(indent_level + 1);
            printf("- key %u\n", *internal_node_key(node, i));
        }
        child = *internal_node_right_child(node);
        print_btree(pager, child, indent_level + 1);
    }
    else if (node_type == NODE_LEAF)
    {
        num_keys = *leaf_node_num_cells(node);
        indent(indent_level);
        printf("- leaf (size %u, page %u, parent %u, next %u)\n",
               num_keys, page_num, *node_parent(node), *leaf_node_next_leaf(node));
        // printf("- leaf (size %d)\n", num_keys);
        for (uint32_t i = 0; i < num_keys; i++)
        {
            indent(indent_level + 1);
            printf("- %u\n", *leaf_node_key(node, i));
        }
    }
    else
    {
        //should not be here
        assert(0);
    }
}

#endif