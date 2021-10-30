run:
	flex calc.l
	bison -d calc.y
	# use 'gcc -fl' if you build on Linux
	gcc -ll calc.tab.c lex.yy.c 
	./a.out
clean:
	rm *.tab.c *.tab.h lex.yy.c a.out