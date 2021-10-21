## tinydb

A tiny and dummy database built by myself.



## Build

This project has only one `.c` file. And you can build it by:

```text
$ make run
```

And it will enter our REPL (Read, Evaluate, Print, Loop) program:

```text
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

From so far, `tinydb` supports these sql statement:

- `insert`
- `select`
- `delete`
- `update`

And it also support some meta commands (for debugging):

- `.help`
- `.exit`
- `.constants`
- `.btree`



## Test

```text
bundle exec rspec
```

