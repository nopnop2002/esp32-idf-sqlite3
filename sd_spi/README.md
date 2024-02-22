# Sqlite3 SDSPI example

Uses the SDSPI peripheral on ESP32 to retrieve from SQLite databases.

# Changes from the original   
- Supports SoC with 2MB flash like ESP32-C series   
- Supports Long file mame   

# Installation
After formatting the SD card to FAT32, copy all files in the data folder.   
```
git clone https://github.com/nopnop2002/esp32-idf-sqlite3
cd esp-idf-sqlite3/sd_spi
idf.py menuconfig
idf.py flash monitor
```
# Hardware requirements   
SD card reader with SPI connection.

# Configuration
![config-top](https://github.com/nopnop2002/esp32-idf-sqlite3/assets/6020549/d6287f8e-c552-4aad-bb52-e625a3a1c455)
![config-app](https://github.com/nopnop2002/esp32-idf-sqlite3/assets/6020549/5396f067-f2b2-44ae-8c06-ae79cf5ca2e7)

# Wireing
|ESP32|ESP32S2/S3|ESP32C2/C3/C6|SPI card pin|Notes|
|:-:|:-:|:-:|:-:|:--|
|GPIO15|GPIO35|GPIO04|MOSI|10k pullup if can't mount|
|GPIO12|GPIO37|GPIO06|MISO||
|GPIO14|GPIO36|GPIO05|SCK||
|GPIO13|GPIO34|GPIO01|CS|||
|3.3V|3.3V|3.3V|VCC|Don't use 5V supply|
|GND|GND|GND|GND||
