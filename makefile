build:
	gcc main.c -o tinydb
run:
	make build
	./tinydb mydb.db
test:
	gcc test.c -o a.out
	./a.out