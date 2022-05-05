## tinydb

![Code Grade](https://api.codiga.io/project/29486/status/svg) ![Code Quality Score](https://api.codiga.io/project/29486/score/svg) 

A tiny and dummy database built by myself.



## Introduction

This project is inspired by sqlite, has a similar architecture with sqlite (but simpler).

<img src = "./docs/img/sqlite.png" style = "width: 40%" />

- `tinydb` is the simplest prototype of database, it only contains **ONE** table (see `table_t` and `row_t` in source file `types.h`), whose schema is `id(uint32_t), username(string), email(string)` .
- `id(int)` is the primary key of our table, we will implement index via B+Tree.
- `sqlparser-main.c` - The entry of tinydb. In this program, I implement an tiny SQL Parser based on Flex & Bison, thus the SQL statements in this program are mostly like sqlite or MySQL. And it supports `where` conditions.

## Build `sqlparser-main.c`

```bash
make build
./tinydb mydb.db
```

In this program, I implement a SQL Parser. It will support SQL statements like:

- `SELECT * FROM table;`
- `SELECT col1, col2 FROM table;`
- `SELECT * FROM table WHERE conditions;`
- `SELECT col1, col2 FROM table WHERE conditions;`
- `INSERT INTO table VALUES (NUMBER, STRING, STRING);`
- `DELETE FROM table WHERE conditions;`
- `UPDATE table SET col1 = val1, col2 = val2;`
- `COMMIT;`
- `ROLLBACK;`

For the above keywords, they can be lower cases: `select, insert, delete, update, ...`

Please note that there is a `';'` after each SQL statement.

And the `conditions` support operators: `=, !=, >=, >, <=, <, AND, OR`, e.g.

```sql
SELECT * FROM table WHERE id < 10 OR id > 100 AND username = 'sinkinben';
SELECT * FROM table WHERE id = 1 AND username='1' OR id >= 10 AND id < 20;
SELECT * FROM table WHERE id = 1 AND username='1' OR id >= 10 AND username < '20';
```

The keywords `AND, OR` can be `and, or`.

Here is an example, showing how to use `tinydb`.

```text
tinydb > insert into table values (1, 'sinkinben', 'skb@qq.com');
Executed.
tinydb > select * from table where id=1 and username='sinkinben';
id = 1 username = sinkinben email = skb@qq.com
total 1 rows
Executed.
tinydb > update table set email='sinkinben@outlook.com' where id=1;
Executed.
tinydb > select * from table;
(1, sinkinben, sinkinben@outlook.com)
total 1 rows
Executed.
tinydb > .exit
```



## Running Test

```bash
mv gemfile Gemfile  # rename Gemfile
bundle install
bundle exec rspec
```

Running this testing script, will fill the database file `mydb.db` with dummy data (total 6000 rows).



## Further Reading

- [1] [SQLite Database System: Design and Implementation](https://play.google.com/store/books/details/SQLite_Database_System_Design_and_Implementation_F?id=9Z6IQQnX1JEC&gl=US)
- [2] [Architecture of SQLite](https://www.sqlite.org/arch.html)
- [3] https://github.com/cstack/db_tutorial

