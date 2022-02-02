%{
#include "sqlparser.tab.h"
%}

%%

"select" |
"SELECT" { return SELECT; }

"create" |
"CREATE" { return CREATE; }

"from" |
"FROM" { return FROM; }

"where" |
"WHERE" { return WHERE; }

"TABLE" { return TABLE; }

"int" |
"INT" { return INT; }

"char"    |
"CHAR"    |
"varchar" |
"VARCHAR" { return CHAR; }



[;] |
[,] |
[*] |
[)] |
[(] { return *yytext; }

">=" { return GREAT_EQUAL; }
">"  { return GREAT; }
"<"  { return LESS; }
"<=" { return LESS_EQUAL; }
"!=" { return NOT_EQUAL; }
"="  { return EQUAL; }
"==" { return EQUAL; }

"and" |
"AND" { return AND; }

"or" |
"OR" { return OR; }

[\'][A-Za-z0-9_@\.]*[\'] {
    /* Remove the single quation marks. */
    int len = strlen(yytext);
    yytext[len - 1] = '\0';
    yylval.strval = strdup(yytext + 1);
    return STRING;
}

[A-Za-z][A-Za-z0-9_]* { yylval.strval = strdup(yytext); return IDNAME; }
[0-9]+                { yylval.intval = atoi(yytext);   return NUMBER; }

\n      { return EOL; }
[ \t]+  { /* ignore and do nothing */ }


%%