## Simlpe SQL Parser Implemented By Flex and Bison

Generally speaking, a compiler has 3 parts:

- Front-end: Lexer/Scanner, Parser, Semantic Analyzer
  - Front-end will produce IR (Internal Result)
- Middle-end: Optimizer
- Back-end: Code Generator
  - Back-end will generate the binary code related to specific machine



## Intro

**Lexer**

Lexer is called "词法分析器" in Chinese. Lex is the process of converting a sequence of characters into a sequence of tokens. For example, if there is an expression `var = 1 - 3 ** 2` in our source code, the lexer should (and it will) output:

```
+--------+----------------------+
| Lexeme |    Token Type        |
+--------+----------------------+
|  var   | Varaible             |
|  =     | Assignment Operator  |
|  1     | Number               |
|  -     | Subtraction Operator |
|  3     | Number               |
|  **    | Power Operator       |
|  2     | Number               |
+--------+----------------------+
```

---

**Parser**

Parser is called "语法分析器" in Chinese. Parsing is the process of analyzing a sequence of tokens to determine its grammatical structure (aka, Abstract Syntax Tree, AST). Syntax errors are identified during this stage.

The parser of a compiler should (and it will) convert the `<lexeme, token type>` pairs into AST:

```
     =
   /   \
 foo    -
      /   \
     1     **
          /   \
         3     2
```

---

**Semantic Analyzer**

Semantic analyzer is called "语义分析器" in Chinese. Semantic analysis is the process of **performing semantic checks**.

So what is the "semantic checks"? For example, in strong type language, type checking, object binding both belong to semantic checks. If we have a declaration `float f = "hello"` , then semantic analyzer should (and it will) output some message like `error:incompatible type ` .

---

**Code Generator**

Code generating is the process by which a compiler's code generator converts some intermediate representation of source code into a form (e.g. machine code) that can be readily executed by a machine.

For example:

```c
int hello() { return -1; }
```

The code generator should output some internal result like assemble code:

```assembly
foo:
    add $esp, -16
    mov $eax, -1
```

---

**Lex v.s. Flex**

- Lex
  - Lex is a tool to generator lexical analyzers.
  - It was written by Mike Lesk and Eric Schmidt (the Google guy).
  - It isn’t used anymore.
- Flex (Fast Lexical analyzer generator)
  - Free and open source alternative.
  - We will use Flex + Bison to implement our tiny sql parser.



---

**Yacc v.s. Bison**

- Yacc
  - Yacc is s a tool to generate parsers (syntactic analyzers).
  - Generated parsers require a lexical analyzer.
  - It isn’t used anymore.
- Bison
  - Free and open source alternative.
  - We will use Flex + Bison to implement our tiny sql parser.

In this article, we will implement a tiny sql parser by Flex and Bison.



## Flex

### Basic Usage

Let's have a look on an simple example `Word Count`.

```c
/* just like Unix wc */
%{
int chars = 0;
int words = 0;
int lines = 0;
%}

%%

[a-zA-Z]+  { words++; chars += strlen(yytext); }
\n         { chars++; lines++; }
.          { chars++; }

%%

int main(int argc, char **argv)
{
  yylex();
  printf("%8d%8d%8d\n", lines, words, chars);
  return 0;
}
```

In above file `flex.l` , it contains 3 sections (each section is separated by `%%` line):

- The first section is **declarations** (surrounded by `%{ /* source code */ %}`), within which shoule be the C source code. This section usually contains declarations and option settings. In this section, we can also include some library like `#include <string.h>` .
- The second section is **rules**, a list of **patterns and actions**, a 'pattern' is a regex, and an 'action' is represented by C source code. The variable `yytext` is the string that matches to the regex pattern.
- The third section is **source code**, including a `main` function.

We can build this `flex.l` file via:

```shell
# it will generate lex.yy.c
> flex flex.l
# On MacOS, -ll means linking flex library (like -lmath, -lpthread).
# On Ubuntu, it should be -lfl.
> gcc lex.yy.c -ll  
> ./a.out 
Hello world, I am skb.  # Ctrl + D
       1       5      23
```

If we want to count the number of a file, we can:

```shell
> ./a.out < lex.yy.c 
    1749    6570   44115
> ./a.out < flex.l 
      20      37     295
```

 

### Tokenizer

Let's see some advaned usages. Now, we will tokenizer a expression in C language.

```c
%{
#include <stdio.h>
// #include "bison.tab.h"
typedef enum 
{
  NUMBER = 258,
  ADD,
  SUB,
  MUL,
  DIV,
  EOL,
} token_type_t;
int yyval;
%}

%%

"+"    { return ADD; }
"-"    { return SUB; }
"*"    { return MUL; }
"/"    { return DIV; }
[0-9]+ { yyval = atoi(yytext); return NUMBER; }
\n     { return EOL; }
[ \t]  {}
.      { printf("Unknow string: %s\n", yytext); }

%%

int main(int argc, char *argv[])
{
  int ret;
  while ((ret = yylex()) != 0) 
  {
    if (ret == NUMBER)
      printf("number = %d, type = %d\n", yyval, ret);
    else if (ret == EOL)
      printf("token = \'\\n\', type = %d\n", ret);
    else
      printf("token = \'%s\', type = %d\n", yytext, ret);
  }
}

```

Build it:

```shell
> flex flex.l; gcc lex.yy.c -ll
> cat input.txt
111 + 222 - 333 * 456 - 789 / 1000
> ./a.out < input.txt
number = 111, type = 258
token = '+', type = 259
number = 222, type = 258
token = '-', type = 260
number = 333, type = 258
token = '*', type = 261
number = 456, type = 258
token = '-', type = 260
number = 789, type = 258
token = '/', type = 262
number = 1000, type = 258
```





## Bison

Suppose we have got all the `<token, type>` pairs, and we want to convert them into an AST, that what Bison can help us do.

```
1 * 2 + 3 * 4 + 5 

        +
      /   \
     +     5
   /   \
  *     *
 / \   /  \
1   2 3    4 
```

### BNF

Backus-Naur Form (BNF), is called "BNF 范式" in some Chinese textbooks. Our BNF example here is very simple (and naivce):

```
<exp>    ::= <factor>
         |   <exp> + <factor>
<factor> ::= NUMBER
         |   <factor> * NUMBER
```

In BNF,  `::=` can be read as "is a", and `|` can be read as "or". For example, the 1st BNF above can be read as "`<exp> is a <factor> or <exp> + <factor>`" .



### Let's Build a Calculator

> The source code of this part can be found in `sqlparser/calcdemo` of the github repo [tinydb](https://github.com/sinkinben/tinydb.git).

In this part, we will show the basic usages of Bison via building a calculator (support `+, -, *, /` ) example.

Bison programs have (not by coincidence) the same three-part structure as flex programs, with **declarations, rules, and C code**. In bison, we replace `::=` in BNF with `:`, and we add a semicolon after each BNF.

For the BNF rules in bison:

- Each symbol in a bison rule has a value, the value of the target symbol (the one to the left of the colon) is called `$$` in the action code. nd the values on the right are numbered 1, 2 and so forth, up to the number of symbols in the rule.
- The values of **tokens** (declared by `%token` line) are whatever was in `yylval` when the scanner returned the token; the values of other symbols are set in rules in the parser. In this parser, the values of the `factor`, `term`, and `exp` symbols are the value of the expression they represent.

```c
// file: calc.y
%{
#include <stdio.h>
#include <stdlib.h>
extern int yylex();
extern int yyparse();
void yyerror(const char *msg)
{
    fprintf(stderr, "error: %s\n", msg);
}

int yywrap() { return 1; }

void prompt() { printf("expr > "); }

int main(int argc, char *argv[])
{
    prompt();
    yyparse();
}
%}

%token NUMBER
%token ADD SUB MUL DIV
%token EOL SPACE EXIT

%%
calculation:
| calculation line { prompt(); }
;

line: EOL
| exp EOL  { printf("%d\n", $1); }
| EXIT EOL { printf("bye!"); exit(0); }
;

exp: factor        { $$ = $1; }
| exp ADD factor   { $$ = $1 + $3; }
| exp SUB factor   { $$ = $1 - $3; }
;

factor: term       { $$ = $1; }
| factor MUL term  { $$ = $1 * $3; }
| factor DIV term  { $$ = $1 / $3; }
;


term: NUMBER { $$ = $1; }
;

%%
```

And we make a simple modification on the flex source file above:

```c
// file: calc.l
%{
#include <stdio.h>
#include "calc.tab.h"
%}

%%

"+"    { return ADD; }
"-"    { return SUB; }
"*"    { return MUL; }
"/"    { return DIV; }
[0-9]+ { yylval = atoi(yytext); return NUMBER; }
\n     { return EOL; }
[ \t]  { }
"exit" { return EXIT; }
.      { printf("Unknow string: %s\n", yytext); }

%%
```

And we build `calc.l` and `calc.y` by `makefile`:

```makefile
run:
	flex calc.l
	bison -d calc.y
	# use 'gcc -fl' if you build on Linux
	gcc -ll calc.tab.c lex.yy.c 
	./a.out
```

Type `make run`, and you will enter the calculator program:

```shell
terminal > ./a.out
expr > 1 + 2 - 3 * 4 + 4 / 4 - 1
-9
expr > 1 + 2
3
expr > exit
bye!        
```



### Advanced Usages

TODO.



## References

- [1] [Flex and Bison Tutorial](https://www.capsl.udel.edu/courses/cpeg421/2012/slides/Tutorial-Flex_Bison.pdf)
- [2] [Introducing to Flex and Bison](https://www.oreilly.com/library/view/flex-bison/9780596805418/ch01.html)

