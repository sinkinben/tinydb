%{
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>
#include "schema.h"
#include "condition.h"

extern int yyparse();
extern int yylex();
void yyerror(const char *);

schema_node_t *schema_list;   // create table tbl (`schema_list`)
schema_node_t *select_list;   // select `select_list` from tbl
condition_t *condition_tree;  // select `select_list` from tbl where `condition_tree`

int yywrap() { return 1; }

void yyerror(const char *msg) { fprintf(stderr, "error: %s\n", msg); }


%}

%union
{
    int32_t intval;
    uint32_t uintval;
    const char *strval;
    struct schema_node_t *schema_node;
    struct schema_node_t *select_node;
    struct condition_t   *condition_tree;
}

// sql keywords
%token SELECT FROM TABLE CREATE WHERE

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
;

selectsql:
    SELECT '*' FROM IDNAME ';'
    {
        printf("TODO: please select * from [%s] \n", $4);
        printf("IDNAME = %p\n", $4);
    }
|   SELECT selectitemlist FROM IDNAME ';'
    {
        printf("TODO: please select ");
        list_node_t *pos;
        list_for_each(pos, &(select_list->entry))
        {
            const char *fieldname = list_entry(pos, schema_node_t, entry)->schema.fieldname;
            printf("%s ", fieldname);
        }
        printf("from %s \n", $4);
        init_list_head(&(select_list->entry));
    }
|   SELECT '*' FROM IDNAME WHERE conditions ';'
    {
        printf("[SELECT] cond ptr = %p, table name = %s \n", $6, $4);
        print_tree($6, 0);
        init_list_head(&(select_list->entry));
        condition_tree = NULL;
    }
|   SELECT selectitemlist FROM IDNAME WHERE conditions ';'
    {
        printf("---> TODO: please select ");
        list_node_t *pos;
        list_for_each(pos, &(select_list->entry))
        {
            const char *fieldname = list_entry(pos, schema_node_t, entry)->schema.fieldname;
            printf("%s ", fieldname);
        }
        printf("from %s \n", $4);
        printf("---> where condition is \n");
        print_tree($6, 0);
        init_list_head(&(select_list->entry));
        condition_tree = NULL;
    }
;

columnitem:
    IDNAME
    {
        $$ = alloc_schema_node($1, 0, -1);
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
        $$ = alloc_condition((uint64_t)(&($1->schema)), $2, $3, true);
        printf("[conditionitem NUMBER]");
        print_tree($$, 0);
    }
|   columnitem cmp_op STRING
    {
        $$ = alloc_condition((uint64_t)(&($1->schema)), $2, (uint64_t)($3), true);
        printf("[conditionitem STRING]");
        print_tree($$, 0);
    }
;

conditions:
    conditionitem
    {
        $$ = $1;
        printf("[conditionitem]");
        print_tree($$, 0);
    }
|   '(' conditions ')'
    {
        $$ = $2;
    }
|   conditions logic_op conditions
    {
        printf("[logic %d] left: ", $2);
        print_tree($1, 0);
        printf("[logic %d] right: ", $2);
        print_tree($3, 0);

        $$ = alloc_condition((uint64_t)($1), $2, (uint64_t)($3), false);
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
        $$ = alloc_schema_node($1, $4, COLUMN_INT);
    }
|   IDNAME CHAR '(' NUMBER ')'
    {
        $$ = alloc_schema_node($1, $4, COLUMN_VARCHAR);
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
    schema_list = alloc_schema_node("", 0, DUMMY);

    /* used by select sql, e.g. select `select_list` from tbl,
     * select_list is a dummy list head node.
     */
    select_list = alloc_schema_node("", 0, DUMMY);

    /* used by where condition, e.g.
     * select `select_list` from tbl where `condition_tree`,
     * here we set it NULL, since we don't need a dummy root.
     */
    condition_tree = NULL;
}
