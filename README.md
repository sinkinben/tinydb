## tinydb

A tiny and dummy database built by myself.



## Build

```text
zsh$ make run
make build
gcc main.c -o tinydb
./tinydb mydb.db

tinydb > insert 1 1 1
Executed.
tinydb > insert 2 2 2
Executed.
tinydb > insert 3 3 3
Executed.
tinydb > update 1 a a
Executed.
tinydb > delete 3
Executed.
tinydb > select
(1, a, a)
(2, 2, 2)
total 2 rows
Executed.
tinydb > 
```



## Test

```text
bundle exec rspec
```

