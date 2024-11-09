# Sqlite3 FATFS example
Creates two Sqlite3 databases on FATFS, inserts and retrieves data from them.

# Changes from the original   
- Supports SoC with 2MB flash like ESP32-C series   
- Supports Long file mame   

# Installation
```
git clone https://github.com/nopnop2002/esp32-idf-sqlite3
cd esp-idf-sqlite3/fatfs
idf.py menuconfig
idf.py flash monitor
```

There is no MENU ITEM where this application is peculiar.


# Partition size
Please change ```partitions.csv``` according to your environment.   

