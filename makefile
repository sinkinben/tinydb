GCC=gcc -Wall
build:
	$(GCC) main.c -o tinydb
clean:
	rm *.db
run:
	make build
	./tinydb mydb.db
test:
	$(GCC) test.c -o a.out
	./a.out
