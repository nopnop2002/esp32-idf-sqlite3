# Sqlite3 Query example

A demo that determines the presence or absence of a table and the presence or absence of a record.   

# How to use

### Clear flash memory and then build
```
$ idf.py erase-flash
$ idf.py flash monitor
```

There is no table, so we will create one.   
```
Opened database successfully
select count(*) from sqlite_master where name = 'test';
Operation done successfully
I (9154) SQLITE3: readBytes=12
I (9154) SQLITE3: sqlmsg=[count(*) = 0]
CREATE TABLE test (id INTEGER, content);
Operation done successfully
W (9284) SQLITE3: Table created
```
There is no record, so we will add one.   
```
select count(*) from test;
Operation done successfully
I (9294) SQLITE3: readBytes=12
I (9294) SQLITE3: sqlmsg=[count(*) = 0]
INSERT INTO test VALUES (1, 'Hello, World');
Operation done successfully
W (9364) SQLITE3: Record inserted
```


### Build again
```
$ idf.py flash monitor
```
Since the table already exists, we will not cretae the table.   
```
Opened database successfully
select count(*) from sqlite_master where name = 'test';
Operation done successfully
I (584) SQLITE3: readBytes=12
I (584) SQLITE3: sqlmsg=[count(*) = 1]
W (584) SQLITE3: Table already exists
```
Since the record already exists, we will not add the record.   
```
select count(*) from test;
Operation done successfully
I (604) SQLITE3: readBytes=12
I (604) SQLITE3: sqlmsg=[count(*) = 1]
W (604) SQLITE3: Record already exists
```
