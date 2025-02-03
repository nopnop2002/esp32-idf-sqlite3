# Sqlite3 Query example

A demo that determines the presence or absence of a table and the presence or absence of a record.   

# How to use

### Clear flash memory and then build
```
$ idf.py erase-flash
$ idf.py flash monitor
```

There is no table, so create one.   
There is no record, so add one.   
```
Opened database successfully
select count(*) from sqlite_master where name = 'test';
Operation done successfully
I (9154) SQLITE3: readBytes=12
I (9154) SQLITE3: sqlmsg=[count(*) = 0]
CREATE TABLE test (id INTEGER, content);
Operation done successfully
W (9284) SQLITE3: Table created
select count(*) from test;
Operation done successfully
I (9294) SQLITE3: readBytes=12
I (9294) SQLITE3: sqlmsg=[count(*) = 0]
INSERT INTO test VALUES (1, 'Hello, World');
Operation done successfully
W (9364) SQLITE3: Record inserted
SELECT * FROM test
Operation done successfully
I (9374) SQLITE3: readBytes=6
I (9374) SQLITE3: sqlmsg=[id = 1]
I (9374) SQLITE3: readBytes=22
I (9374) SQLITE3: sqlmsg=[content = Hello, World]
I (10384) SQLITE3: readBytes=0
All Done
```


### Build again
```
$ idf.py flash monitor
```
Since the table already exists, we will not cretae the table.   
Since the record already exists, we will not add the record.   
```
Opened database successfully
select count(*) from sqlite_master where name = 'test';
Operation done successfully
I (584) SQLITE3: readBytes=12
I (584) SQLITE3: sqlmsg=[count(*) = 1]
W (584) SQLITE3: Table already exists
select count(*) from test;
Operation done successfully
I (604) SQLITE3: readBytes=12
I (604) SQLITE3: sqlmsg=[count(*) = 1]
W (604) SQLITE3: Record already exists
SELECT * FROM test
Operation done successfully
I (624) SQLITE3: readBytes=6
I (624) SQLITE3: sqlmsg=[id = 1]
I (624) SQLITE3: readBytes=22
I (624) SQLITE3: sqlmsg=[content = Hello, World]
I (634) main_task: Returned from app_main()
I (1634) SQLITE3: readBytes=0
All Done
```
