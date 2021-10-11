#ifndef DECLARATION_H
#define DECLARATION_H
/**
 * 1. 某些文件存在函数的循环引用, 因此引入这一文件作为函数的声明, 破除循环 include 
 * 2. 工程上并不是一个好的设计, tinydb 为了方便, 把所有的函数实现都写在了 header file
 **/
void serialize_row(row_t *source, void *dest);
#endif