CC=clang -Wall -Wno-pointer-to-int-cast
build:
	$(CC) main.c -o tinydb
clean:
	rm *.db
run:
	make build
	./tinydb mydb.db
test:
	$(CC) test.c -o a.out
	./a.out
