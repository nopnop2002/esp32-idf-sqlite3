# Sqlite3 LittleFS example
Demo of using LittleFS file system.   
Creates two Sqlite3 databases on LittleFS, inserts and retrieves data from them.

# Changes from the original   
- Newly created   

# Installation
```
git clone https://github.com/nopnop2002/esp32-idf-sqlite3
cd esp32-idf-sqlite3/littlefs
idf.py menuconfig
idf.py flash monitor
```

There is no MENU ITEM where this application is peculiar.

# Partition size
To support the ESP32-C series, this project uses a partition of 0xF0000 (=960K) by default.   
If your ESP32 has more than 4M Flash, you can change ```partitions.csv```.   
