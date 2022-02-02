#ifndef GLOBAL_H
#define GLOBAL_H
/**
 * 1. 某些头文件存在循环引用, 因此引入这一文件作为函数的全局声明, 破除循环 include
 * 2. 工程上并不是一个好的设计, tinydb 为了方便, 把所有的函数实现都写在了头文件
 **/
void serialize_row(row_t *source, void *dest);
void *cursor_value(cursor_t *cursor);
void print_btree(pager_t *pager, uint32_t page_num, uint32_t indent_level);

#define TODO(filename, line_num)                                          \
    do {                                                                  \
        printf("\033[31m TODO at %s: %d \033[0m \n", filename, line_num); \
        assert(0);                                                        \
    } while (0)

// #define CUSTOMED_LEAF_MAX_CELLS ((uint32_t)3)
// #define CUSTOMED_INTERNAL_MAX_CELLS ((uint32_t)3)

#define OPEN_TRANSACTION (1)

#define transaction_fatal()                    \
    {                                          \
        printf("Transaction is not opened\n"); \
        assert(0);                             \
    }

#endif