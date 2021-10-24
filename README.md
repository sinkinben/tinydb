## tinydb

![Code Grade](https://www.code-inspector.com/project/29486/status/svg) ![Code Quality Score](https://www.code-inspector.com/project/29486/score/svg) 

A tiny and dummy database built by myself.



## Introdution

- `tinydb` is the simplest prototype of database, it only contains **ONE** table (see `table_t` and `row_t` in source file `types.h`), whose schema is `id(uint32_t), username(string), email(string)` .
- `id(int)` is the primary key of our table, we will implement index via B+Tree.



This project is inspired by sqlite.

<div style="margin-left: auto; display: inline-block;"><svg xmlns="http://www.w3.org/2000/svg" class="pikchr" width="367" height="453" viewBox="0 0 490.32 605.52">
<path d="M2,293L236,293L236,2L2,2Z" style="fill:rgb(216,236,208);stroke-width:2.16;stroke:rgb(0,0,0);"></path>
<path d="M2,603L236,603L236,311L2,311Z" style="fill:rgb(208,236,232);stroke-width:2.16;stroke:rgb(0,0,0);"></path>
<path d="M254,347L488,347L488,2L254,2Z" style="fill:rgb(232,216,208);stroke-width:2.16;stroke:rgb(0,0,0);"></path>
<path d="M254,603L488,603L488,394L254,394Z" style="fill:rgb(224,236,200);stroke-width:2.16;stroke:rgb(0,0,0);"></path>
<path d="M38,92L200,92L200,38L38,38Z" style="fill:rgb(255,255,255);stroke-width:2.16;stroke:rgb(0,0,0);"></path>
<text x="119" y="65" text-anchor="middle" fill="rgb(0,0,0)" font-size="110%" dominant-baseline="central">Interface</text>
<polygon points="119,120 114,109 123,109" style="fill:rgb(0,0,0)"></polygon>
<path d="M119,92L119,115" style="fill:none;stroke-width:2.16;stroke:rgb(0,0,0);"></path>
<path d="M38,174L200,174L200,120L38,120Z" style="fill:rgb(255,255,255);stroke-width:2.16;stroke:rgb(0,0,0);"></path>
<text x="119" y="135" text-anchor="middle" fill="rgb(0,0,0)" font-size="110%" dominant-baseline="central">SQL&nbsp;Command</text>
<text x="119" y="160" text-anchor="middle" fill="rgb(0,0,0)" font-size="110%" dominant-baseline="central">Processor</text>
<polygon points="119,203 114,192 123,192" style="fill:rgb(0,0,0)"></polygon>
<path d="M119,174L119,198" style="fill:none;stroke-width:2.16;stroke:rgb(0,0,0);"></path>
<path d="M38,257L200,257L200,203L38,203Z" style="fill:rgb(255,255,255);stroke-width:2.16;stroke:rgb(0,0,0);"></path>
<text x="119" y="230" text-anchor="middle" fill="rgb(0,0,0)" font-size="110%" dominant-baseline="central">Virtual&nbsp;Machine</text>
<polygon points="119,347 114,336 123,336" style="fill:rgb(0,0,0)"></polygon>
<path d="M119,257L119,342" style="fill:none;stroke-width:2.16;stroke:rgb(0,0,0);"></path>
<path d="M38,401L200,401L200,347L38,347Z" style="fill:rgb(255,255,255);stroke-width:2.16;stroke:rgb(0,0,0);"></path>
<text x="119" y="374" text-anchor="middle" fill="rgb(0,0,0)" font-size="110%" dominant-baseline="central">B-Tree</text>
<polygon points="119,430 114,419 123,419" style="fill:rgb(0,0,0)"></polygon>
<path d="M119,401L119,424" style="fill:none;stroke-width:2.16;stroke:rgb(0,0,0);"></path>
<path d="M38,484L200,484L200,430L38,430Z" style="fill:rgb(255,255,255);stroke-width:2.16;stroke:rgb(0,0,0);"></path>
<text x="119" y="457" text-anchor="middle" fill="rgb(0,0,0)" font-size="110%" dominant-baseline="central">Pager</text>
<polygon points="119,513 114,501 123,501" style="fill:rgb(0,0,0)"></polygon>
<path d="M119,484L119,507" style="fill:none;stroke-width:2.16;stroke:rgb(0,0,0);"></path>
<path d="M38,567L200,567L200,513L38,513Z" style="fill:rgb(255,255,255);stroke-width:2.16;stroke:rgb(0,0,0);"></path>
<text x="119" y="540" text-anchor="middle" fill="rgb(0,0,0)" font-size="110%" dominant-baseline="central">OS&nbsp;Interface</text>
<path d="M290,92L452,92L452,38L290,38Z" style="fill:rgb(255,255,255);stroke-width:2.16;stroke:rgb(0,0,0);"></path>
<text x="371" y="65" text-anchor="middle" fill="rgb(0,0,0)" font-size="110%" dominant-baseline="central">Tokenizer</text>
<polygon points="371,120 366,109 375,109" style="fill:rgb(0,0,0)"></polygon>
<path d="M371,92L371,115" style="fill:none;stroke-width:2.16;stroke:rgb(0,0,0);"></path>
<path d="M290,174L452,174L452,120L290,120Z" style="fill:rgb(255,255,255);stroke-width:2.16;stroke:rgb(0,0,0);"></path>
<text x="371" y="147" text-anchor="middle" fill="rgb(0,0,0)" font-size="110%" dominant-baseline="central">Parser</text>
<polygon points="371,203 366,192 375,192" style="fill:rgb(0,0,0)"></polygon>
<path d="M371,174L371,198" style="fill:none;stroke-width:2.16;stroke:rgb(0,0,0);"></path>
<path d="M290,311L452,311L452,203L290,203Z" style="fill:rgb(255,255,255);stroke-width:2.16;stroke:rgb(0,0,0);"></path>
<text x="371" y="245" text-anchor="middle" fill="rgb(0,0,0)" font-size="110%" dominant-baseline="central">Code</text>
<text x="371" y="270" text-anchor="middle" fill="rgb(0,0,0)" font-size="110%" dominant-baseline="central">Generator</text>
<path d="M290,484L452,484L452,430L290,430Z" style="fill:rgb(255,255,255);stroke-width:2.16;stroke:rgb(0,0,0);"></path>
<text x="371" y="457" text-anchor="middle" fill="rgb(0,0,0)" font-size="110%" dominant-baseline="central">Utilities</text>
<path d="M290,567L452,567L452,513L290,513Z" style="fill:rgb(255,255,255);stroke-width:2.16;stroke:rgb(0,0,0);"></path>
<text x="371" y="540" text-anchor="middle" fill="rgb(0,0,0)" font-size="110%" dominant-baseline="central">Test&nbsp;Code</text>
<polygon points="290,78 281,87 277,79" style="fill:rgb(0,0,0)"></polygon>
<path d="M200,120L284,81" style="fill:none;stroke-width:2.16;stroke:rgb(0,0,0);"></path>
<polygon points="200,174 212,178 206,185" style="fill:rgb(0,0,0)"></polygon>
<path d="M290,239L204,178" style="fill:none;stroke-width:2.16;stroke:rgb(0,0,0);"></path>
<text x="20" y="147" text-anchor="middle" font-style="italic" fill="rgb(0,0,0)" font-size="110%" transform="rotate(-90 20,147)" dominant-baseline="central">Core</text>
<text x="20" y="457" text-anchor="middle" font-style="italic" fill="rgb(0,0,0)" font-size="110%" transform="rotate(-90 20,457)" dominant-baseline="central">Backend</text>
<text x="470" y="174" text-anchor="middle" font-style="italic" fill="rgb(0,0,0)" font-size="110%" transform="rotate(-90 470,174)" dominant-baseline="central">SQL&nbsp;Compiler</text>
<text x="470" y="498" text-anchor="middle" font-style="italic" fill="rgb(0,0,0)" font-size="110%" transform="rotate(-90 470,498)" dominant-baseline="central">Accessories</text>
</svg></div>


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
- `commit`
- `rollback`

And it also support some meta commands (for debugging):

- `.help`
- `.exit`
- `.constants`
- `.btree`



## Test

```bash
$ mv gemfile Gemfile  # rename Gemfile
$ bundle install
$ bundle exec rspec
```



## Recommend Readings

- [1] [SQLite Database System: Design and Implementation](https://play.google.com/store/books/details/SQLite_Database_System_Design_and_Implementation_F?id=9Z6IQQnX1JEC&gl=US)
- [2] [Architecture of SQLite](https://www.sqlite.org/arch.html)

