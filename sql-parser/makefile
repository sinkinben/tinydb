CC=clang -std=gnu17 -Wall -Wno-pointer-to-int-cast
build:
	flex sqlex.l
	bison -d sqlparser.y
	# use 'gcc -fl' if you build on Linux
	$(CC) sqlparser.tab.c lex.yy.c -ll

clean:
	rm a.out lex.yy.c *.tab.c *.tab.h