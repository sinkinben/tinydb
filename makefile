CC=clang -Wall -Wno-pointer-to-int-cast -Wno-unused-function

PROJECT=$(shell pwd)
SQLPARSER=$(PROJECT)/sql-parser
SQLVM=$(PROJECT)/sql-v,

INCLUDE=-I$(PROJECT) -I$(SQLPARSER) -I$(SQLVM)

build: sqlparser
	$(CC) $(INCLUDE) sqlparser.tab.c lex.yy.c main.c -o tinydb

clean:
	rm *.db tinydb lex.yy.c *.tab.c *.tab.h

run:
	make build
	./tinydb mydb.db

sqlparser:
	flex $(SQLPARSER)/sqlex.l
	bison -d $(SQLPARSER)/sqlparser.y

test:
	$(CC) test.c -o a.out
	./a.out
