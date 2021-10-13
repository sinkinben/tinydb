#ifndef GLOBAL_H
#define GLOBAL_H
/**
 * 1. 某些头文件的循环引用, 因此引入这一文件作为函数的全局声明, 破除循环 include 
 * 2. 工程上并不是一个好的设计, tinydb 为了方便, 把所有的函数实现都写在了 header file
 **/
void serialize_row(row_t *source, void *dest);

void TODO(const char *filename, int line_num)
{
    printf("TODO at %s: %d\n", filename, line_num);
    assert(0);
}
#endif