# Sqlite3 subquery example
Demo of using a subquery with the SPIFFS file system.   
Creates Sqlite3 databases on SPIFFS, inserts and retrieves data from them.

# Changes from the original   
- Supports SoC with 2MB flash like ESP32-C series   

# Installation
```
git clone https://github.com/nopnop2002/esp32-idf-sqlite3
cd esp32-idf-sqlite3/subquery2
idf.py menuconfig
idf.py flash monitor
```

There is no MENU ITEM where this application is peculiar.

# Partition size
To support the ESP32-C series, this project uses a partition of 0xF0000 (=960K) by default.   
If your ESP32 has more than 4M Flash, you can change ```partitions.csv```.   

# Create Table
```
CREATE TABLE Price(flower_ID integer primary key, flower_fruit text, family_ID integer, price integer);
```

# Insert data
```
INSERT INTO Price (flower_ID,flower_fruit,family_ID,price) VALUES (10,'rose',10,150);
INSERT INTO Price (flower_ID,flower_fruit,family_ID,price) VALUES (20,'azalea',20,200);
INSERT INTO Price (flower_ID,flower_fruit,family_ID,price) VALUES (30,'orange',30,500);
INSERT INTO Price (flower_ID,flower_fruit,family_ID,price) VALUES (1,'apple',10,120);
INSERT INTO Price (flower_ID,flower_fruit,family_ID,price) VALUES (2,'grapefruit',30,150);
INSERT INTO Price (flower_ID,flower_fruit,family_ID,price) VALUES (3,'rhododendron',20,250);
INSERT INTO Price (flower_ID,flower_fruit,family_ID,price) VALUES (4,'blueberry',20,370);
INSERT INTO Price (flower_ID,flower_fruit,family_ID,price) VALUES (5,'plum',10,400);
INSERT INTO Price (flower_ID,flower_fruit,family_ID,price) VALUES (6,'peach',10,390);
```

# Query Price
```
SELECT * FROM Price;
Callback function called: flower_ID = 1
flower_fruit = apple
family_ID = 10
price = 120

Callback function called: flower_ID = 2
flower_fruit = grapefruit
family_ID = 30
price = 150

Callback function called: flower_ID = 3
flower_fruit = rhododendron
family_ID = 20
price = 250

Callback function called: flower_ID = 4
flower_fruit = blueberry
family_ID = 20
price = 370

Callback function called: flower_ID = 5
flower_fruit = plum
family_ID = 10
price = 400

Callback function called: flower_ID = 6
flower_fruit = peach
family_ID = 10
price = 390

Callback function called: flower_ID = 10
flower_fruit = rose
family_ID = 10
price = 150

Callback function called: flower_ID = 20
flower_fruit = azalea
family_ID = 20
price = 200

Callback function called: flower_ID = 30
flower_fruit = orange
family_ID = 30
price = 500
```

# Subquery
```
Select flower_fruit, price from price where price = (select max(price) from price);
Callback function called: flower_fruit = orange
price = 500
```
