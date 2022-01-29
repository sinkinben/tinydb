#ifndef __SQL_PARSER_H__
#define __SQL_PARSER_H__

typedef struct yy_buffer_state * YY_BUFFER_STATE;
extern YY_BUFFER_STATE yy_scan_string(const char *);
extern void yy_delete_buffer(YY_BUFFER_STATE);
extern int yyparse();

static int sql_parser(const char *sql)
{
    YY_BUFFER_STATE buffer = yy_scan_string(sql);
    int ret = yyparse();  /* 0 - Success, 1 - Fail */
    yy_delete_buffer(buffer);
    return ret;
}

#endif
