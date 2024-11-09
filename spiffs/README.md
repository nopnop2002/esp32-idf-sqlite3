# Sqlite3 SPIFFS example
Creates two Sqlite3 databases on SPIFFS, inserts and retrieves data from them.

# Changes from the original   
- Supports SoC with 2MB flash like ESP32-C series   

# Installation
```
git clone https://github.com/nopnop2002/esp32-idf-sqlite3
cd esp-idf-sqlite3/spiffs
idf.py menuconfig
idf.py flash monitor
```

There is no MENU ITEM where this application is peculiar.

# Partition size
To support the ESP32-C series, this project uses a partition of 0xF0000 (=960K) by default.   
If your ESP32 has more than 4M Flash, you can change ```partitions.csv```.   
