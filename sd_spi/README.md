# Sqlite3 SDSPI example
Demo of using FATFS file system on SDSPI peripheral.   
Uses the SDSPI peripheral on ESP32 to retrieve from SQLite databases.

# Changes from the original   
- Supports SoC with 2MB flash like ESP32-C series   
- Supports Long file mame   

# Installation
After formatting the SD card to FAT32, copy all files in the data folder.   
```
git clone https://github.com/nopnop2002/esp32-idf-sqlite3
cd esp32-idf-sqlite3/sd_spi
idf.py menuconfig
idf.py flash monitor
```
# Hardware requirements   
SD card reader with SPI connection.   
SD card readers come in two types: those that operate on 5V and those that operate on 3.3V.   
Requires hardware powered by 3.3V.   

# Configuration
![config-top](https://github.com/nopnop2002/esp32-idf-sqlite3/assets/6020549/d6287f8e-c552-4aad-bb52-e625a3a1c455)
![Image](https://github.com/user-attachments/assets/269cd575-992f-4380-9e15-ac7e9f0eb255)

# Wireing
Click [here](https://github.com/espressif/esp-idf/tree/master/examples/storage/sd_card/sdspi) for details.   

|SPI pin||ESP32|ESP32S2/S3|ESP32Cx/Hx|Notes|
|:-:|:-:|:-:|:-:|:-:|:-:|
|MISO||GPIO19|GPIO37|GPIO03||
|CS||GPIO5|GPIO34|GPIO01||
|SCK||GPIO18|GPIO36|GPIO02||
|MOSI||GPIO23|GPIO35|GPIO04|10k pullup if can't mount|
|VCC||3.3V|3.3V|3.3V|Don't use 5V supply|
|GND||GND|GND|GND||
