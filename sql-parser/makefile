CC=clang -std=gnu17 -Wall -Wno-pointer-to-int-cast
build:
	flex sqlex.l
	bison -d sqlparser.y

clean:
	rm a.out lex.yy.c *.tab.c *.tab.h

demo: build
	# use 'gcc -fl' if you build on Linux
	$(CC) sqlparser.tab.c lex.yy.c demo.c -ll
	./a.out
