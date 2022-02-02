%{
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>
#include "schema.h"
#include "condition.h"
#include "sql-statement/statement.h"

extern int yyparse();
extern int yylex();
void yyerror(const char *);

schema_node_t *schema_list;   // create table tbl (`schema_list`)
schema_node_t *select_list;   // select `select_list` from tbl
schema_node_t *update_list;   // update table set `update_list` where `condition_tree`
condition_t *condition_tree;  // select `select_list` from tbl where `condition_tree`

int yywrap() { return 1; }

void yyerror(const char *msg) { fprintf(stderr, "[tinydb] SQL Parser: %s\n", msg); }

#define YYPARSE_PARAM parm

/* `stm_ptr` is a pointer, that points to a statement_t struct,
 * see `sql_parser()` in sqlparser.h
 */
#define stm_ptr ((statement_t *)(parm))

%}

%union
{
    int32_t intval;
    uint32_t uintval;
    const char *strval;
    struct schema_node_t *schema_node;
    struct schema_node_t *select_node;
    struct schema_node_t *update_node;
    struct condition_t   *condition_tree;
}

// sql keywords
%token SELECT INSERT CREATE DELETE UPDATE
%token FROM TABLE WHERE VALUES INTO SET
%token COMMIT ROLLBACK

// sql operator
%token GREAT GREAT_EQUAL LESS LESS_EQUAL
%token EQUAL NOT_EQUAL
%token AND OR
%token EOL

// token's value
%token<strval> STRING IDNAME INT CHAR
%token<intval> NUMBER

// BNF's value
%type <schema_node> createitem
%type <schema_node> createitemlist
%type <select_node> columnitem
%type <select_node> selectitemlist
%type <update_node> updateitem
%type <update_node> updateitemlist

%type <intval> cmp_op logic_op
%type <condition_tree> conditionitem
%type <condition_tree> conditions


%%

statements:
    statements statement  {}
|   statement {}
;

statement:
    EOL {}
|   selectsql {}
|   createsql {}
|   insertsql {}
|   deletesql {}
|   updatesql {}
|   commitsql {}
|   rollbacksql {}
;

updatesql:
    UPDATE IDNAME SET updateitemlist WHERE conditions ';'
    {
        statement_set(stm_ptr, STATEMENT_UPDATE_WHERE, $2, update_list, $6);
    }
;

deletesql:
    DELETE FROM IDNAME WHERE conditions ';'
    {
        statement_set(stm_ptr, STATEMENT_DELETE_WHERE, $3, select_list, $5);
    }
;

commitsql:
    COMMIT ';'
    {
        statement_set(stm_ptr, STATEMENT_COMMIT, NULL, select_list, NULL);
    }
;

rollbacksql:
    ROLLBACK ';'
    {
        statement_set(stm_ptr, STATEMENT_ROLLBACK, NULL, select_list, NULL);
    }
;

insertsql:
    INSERT INTO IDNAME VALUES '(' NUMBER ',' STRING ',' STRING ')' ';'
    {
        statement_set(stm_ptr, STATEMENT_INSERT, $3, select_list, NULL);
        row_t *row = &(stm_ptr->row_value);
        row->id = $6;
        strcpy(row->username, $8);
        strcpy(row->email, $10);
        free((void *)($8));
        free((void *)($10));
    }
;

selectsql:
    SELECT '*' FROM IDNAME ';'
    {
        statement_set(stm_ptr, STATEMENT_SELECT, $4, select_list, NULL);
    }
|   SELECT selectitemlist FROM IDNAME ';'
    {
        statement_set(stm_ptr, STATEMENT_SELECT_WHERE, $4, select_list, NULL);
    }
|   SELECT '*' FROM IDNAME WHERE conditions ';'
    {
        statement_set(stm_ptr, STATEMENT_SELECT_WHERE, $4, select_list, $6);
    }
|   SELECT selectitemlist FROM IDNAME WHERE conditions ';'
    {
        statement_set(stm_ptr, STATEMENT_SELECT_WHERE, $4, select_list, $6);
    }
;

updateitem:
    IDNAME EQUAL NUMBER
    {
        $$ = alloc_schema_node($1, 4, COLUMN_INT, $3);
        free((void *)($1));
    }
|   IDNAME EQUAL STRING
    {
        $$ = alloc_schema_node($1, 4, COLUMN_VARCHAR, (uint64_t)($3));
        free((void *)($1));
    }
;

updateitemlist:
    updateitem
    {
        list_add_tail(&($1->entry), &(update_list->entry));
    }
|   updateitemlist ',' updateitem
    {
        list_add_tail(&($3->entry), &(update_list->entry));
    }
;

columnitem:
    IDNAME
    {
        $$ = alloc_schema_node($1, 0, COLUMN_DUMMY, 0);
        free((void *)($1));
    }
;

selectitemlist:
    columnitem
    {
        list_add_tail(&($1->entry), &(select_list->entry));
    }
|   selectitemlist ',' columnitem
    {
        list_add_tail(&($3->entry), &(select_list->entry));
    }
;

logic_op:
    AND         { $$ = OP_AND; }
|   OR          { $$ = OP_OR; }

cmp_op:
    LESS        { $$ = OP_LESS; }
|   LESS_EQUAL  { $$ = OP_LESS_EQUAL; }
|   GREAT       { $$ = OP_GREAT; }
|   GREAT_EQUAL { $$ = OP_GREAT_EQUAL; }
|   EQUAL       { $$ = OP_EQUAL; }
|   NOT_EQUAL   { $$ = OP_NOT_EQUAL; }

conditionitem:
    columnitem cmp_op NUMBER
    {
        $$ = alloc_condition((uint64_t)(&($1->schema)), $2, $3, true, COLUMN_INT);
    }
|   columnitem cmp_op STRING
    {
        $$ = alloc_condition((uint64_t)(&($1->schema)), $2, (uint64_t)($3), true, COLUMN_VARCHAR);
    }
;

conditions:
    conditionitem
    {
        $$ = $1;
    }
|   '(' conditions ')'
    {
        $$ = $2;
    }
|   conditions logic_op conditions
    {
        $$ = alloc_condition((uint64_t)($1), $2, (uint64_t)($3), false, COLUMN_DUMMY);
    }
;

createsql:
    CREATE TABLE IDNAME '(' createitemlist ')' ';'
    {
        /* TODO: When after creating table, we should free the `createitemlist` */
        printf("table name = %s\n", $3);
        list_node_t *pos;
        list_for_each(pos, &(schema_list->entry))
        {
            schema_t *schema = &(list_entry(pos, schema_node_t, entry)->schema);
            printf("field: %s %d %s\n", schema->fieldname, schema->width,
                   schema->column_type == COLUMN_INT ? "int" : "varchar");
        }
        printf("TODO: please implement create table sql\n");
        assert(0);
    }
;


createitem:
    IDNAME INT '(' NUMBER ')'
    {
        $$ = alloc_schema_node($1, $4, COLUMN_INT, 0);
    }
|   IDNAME CHAR '(' NUMBER ')'
    {
        $$ = alloc_schema_node($1, $4, COLUMN_VARCHAR, 0);
    }

;

createitemlist:
    createitem
    {
        list_add_tail(&($1->entry), &(schema_list->entry));
    }
|   createitemlist ',' createitem
    {
        list_add_tail(&($3->entry), &(schema_list->entry));
    }
;



%%
void __attribute__((constructor)) init()
{
    /* used by create sql, e.g. create table tbl (`schema_list`),
     * schema_list is a dummy list head node.
     */
    schema_list = alloc_schema_node("", 0, COLUMN_DUMMY, 0);

    /* used by select sql, e.g. select `select_list` from tbl,
     * select_list is a dummy list head node.
     */
    select_list = alloc_schema_node("", 0, COLUMN_DUMMY, 0);

    /* used by update sql, e.g. update table set `schema = schema_value`
     * update_list is a dummy list head node.
     */
    update_list = alloc_schema_node("", 0, COLUMN_DUMMY, 0);

    /* used by where condition, e.g.
     * select `select_list` from tbl where `condition_tree`,
     * here we set it NULL, since we don't need a dummy root.
     */
    condition_tree = NULL;
}
