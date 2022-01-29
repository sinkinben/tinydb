/**
 * @file demo.c
 * @author sinkinben@outlook.com
 * @brief A demo to show the usage of `yyparse()`
 * @date 2022-01-28
 *
 * @copyright Copyright (c) 2022
 *
 *
 * SELECT:
 *   select * from tblname;
 *   select c1, c2, c3, c4 from tblname;
 *   select * from tblid where a != 'ss3' and b > 10 and c <= 9 and d == 'aa';
 *   select c1, c2, c3,c4 from tblid where a != 'ss3' and b > 10 and c <= 9 and d == 'aa' or e >= 16;
 * CREATE:
 *   create table tbl1 (c1 int(10), c2 char(16), c3 int(32));
 *   create table mytable (id int(16), msg1 varchar(32), msg2 varchar(64));
 * TODO:
 *   1) Free the resource, when the SQL parsing processes are finished,
 *      e.g. schema_list, select_list, and condition_tree.
 *   2) The priority between 'AND' and 'OR' is ignored, since I forget how to fix shift/reduce confilcts :-(.
 *      Run: bison -d -v sqlparser.y, in the 'sqlparser.output', we can see the shift/reduce conflicts
 *      For example:
 *      + A AND B OR C => A AND (B OR C), which should be (A AND B) OR C
 *      + A AND B OR C AND D => A AND (B OR (C AND D)), which should be (A AND B) OR (C AND D)
 *      + This is a deadly disadvantage for this dummy sql parser.
 */
#include <stdio.h>
#include <stdint.h>
#include "sqlparser.tab.h"

typedef struct yy_buffer_state * YY_BUFFER_STATE;
extern YY_BUFFER_STATE yy_scan_string(const char *);
extern void yy_delete_buffer(YY_BUFFER_STATE);
extern int yyparse();

int main(int argc, char *argv[])
{
    size_t len;
    char *ptr = NULL;
    while (1)
    {
        printf("tinydb > ");
        getline(&ptr, &len, stdin);
        ptr[len - 2] = '\0';

        YY_BUFFER_STATE buffer = yy_scan_string(ptr);
        /* 0 - Success, 1 - Fail */
        yyparse();
        yy_delete_buffer(buffer);
    }
}