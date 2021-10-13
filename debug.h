#ifndef DEBUG_H
#define DEBUG_H
void print_constants()
{
    printf("ROW_SIZE: %d\n", ROW_SIZE);
    printf("COMMON_NODE_HEADER_SIZE: %d\n", COMMON_NODE_HEADER_SIZE);
    printf("LEAF_NODE_HEADER_SIZE: %d\n", LEAF_NODE_HEADER_SIZE);
    printf("LEAF_NODE_CELL_SIZE: %d\n", LEAF_NODE_CELL_SIZE);
    printf("LEAF_NODE_SPACE_FOR_CELLS: %d\n", LEAF_NODE_SPACE_FOR_CELLS);
    printf("LEAF_NODE_MAX_CELLS: %d\n", LEAF_NODE_MAX_CELLS);
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
        printf("- internal (size %d)\n", num_keys);
        for (uint32_t i = 0; i < num_keys; i++)
        {
            child = *internal_node_child(node, i);
            print_btree(pager, child, indent_level + 1);

            indent(indent_level + 1);
            printf("- key %d\n", *internal_node_key(node, i));
        }
        child = *internal_node_right_child(node);
        print_btree(pager, child, indent_level + 1);
    }
    else if (node_type == NODE_LEAF)
    {
        num_keys = *leaf_node_num_cells(node);
        indent(indent_level);
        printf("- leaf (size %d)\n", num_keys);
        for (uint32_t i = 0; i < num_keys; i++)
        {
            indent(indent_level + 1);
            printf("- %d\n", *leaf_node_key(node, i));
        }
    }
    else
    {
        //should not be here
        assert(0);
    }
}

#endif