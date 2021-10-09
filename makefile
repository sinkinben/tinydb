run:
	gcc main.c -o tinydb
	./tinydb
test:
	gcc test.c -o a.out
	./a.out