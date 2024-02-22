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
