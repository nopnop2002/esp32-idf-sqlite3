# Sqlite3 subquery example
Demo of using a subquery with the SPIFFS file system.   
Creates Sqlite3 databases on SPIFFS, inserts and retrieves data from them.

# Changes from the original   
- Supports SoC with 2MB flash like ESP32-C series   

# Installation
```
git clone https://github.com/nopnop2002/esp32-idf-sqlite3
cd esp32-idf-sqlite3/subquery
idf.py menuconfig
idf.py flash monitor
```

There is no MENU ITEM where this application is peculiar.

# Partition size
To support the ESP32-C series, this project uses a partition of 0xF0000 (=960K) by default.   
If your ESP32 has more than 4M Flash, you can change ```partitions.csv```.   

# Create Table
```
CREATE TABLE departments ( id INTEGER PRIMARY KEY, name TEXT, location TEXT);
CREATE TABLE employees ( id INTEGER PRIMARY KEY, name TEXT, department_id INTEGER);
```

# Insert data
```
INSERT INTO departments (id, name, location) VALUES (1, 'Engineering', 'New York');
INSERT INTO departments (id, name, location) VALUES (2, 'Marketing', 'London');
INSERT INTO departments (id, name, location) VALUES (3, 'Sales', 'New York');
INSERT INTO employees (name, department_id) VALUES ('Alice', 1);
INSERT INTO employees (name, department_id) VALUES ('Bob', 2);
INSERT INTO employees (name, department_id) VALUES ('Charlie', 3);
INSERT INTO employees (name, department_id) VALUES ('David', 1);
```

# Query departments
```
SELECT * FROM departments;
Callback function called: id = 1
name = Engineering
location = New York

Callback function called: id = 2
name = Marketing
location = London

Callback function called: id = 3
name = Sales
location = New York
```

# Query employees
```
SELECT * FROM employees;
Callback function called: id = 1
name = Alice
department_id = 1

Callback function called: id = 2
name = Bob
department_id = 2

Callback function called: id = 3
name = Charlie
department_id = 3

Callback function called: id = 4
name = David
department_id = 1
```

# Subquery
```
SELECT name FROM employees WHERE department_id IN ( SELECT id FROM departments WHERE location = 'New York');
Callback function called: name = Alice

Callback function called: name = Charlie

Callback function called: name = David

SELECT name FROM employees WHERE department_id IN (1, 3);
Callback function called: name = Alice

Callback function called: name = Charlie

Callback function called: name = David

SELECT employees.name FROM employees INNER JOIN departments ON employees.department_id = departments.id WHERE departments.location = 'New York';
Callback function called: name = Alice

Callback function called: name = Charlie

Callback function called: name = David
```
