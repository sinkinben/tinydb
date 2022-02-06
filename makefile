CC=clang -Wall -Wno-pointer-to-int-cast -Wno-unused-function

PROJECT=$(shell pwd)
SQLPARSER=$(PROJECT)/sql-parser
SQLVM=$(PROJECT)/sql-vm
LIBS=$(PROJECT)/libs
DBKERNEL=$(PROJECT)/db-kernel

INCLUDE=-I$(PROJECT) -I$(SQLPARSER) -I$(SQLVM) -I$(LIBS) -I$(DBKERNEL)

build: sqlparser
	$(CC) $(INCLUDE) sqlparser.tab.c lex.yy.c main2.c -o tinydb

clean:
	rm *.db tinydb lex.yy.c *.tab.c *.tab.h

run:
	make build
	./tinydb mydb.db

sqlparser:
	flex $(SQLPARSER)/sqlex.l
	bison -d $(SQLPARSER)/sqlparser.y

test:
	$(CC) $(INCLUDE) main.c -o tinydb
