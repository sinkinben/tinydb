run:
	gcc main.c -o tinydb
	./tinydb mydb.db
test:
	gcc test.c -o a.out
	./a.out