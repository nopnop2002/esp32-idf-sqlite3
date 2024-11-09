# Sqlite3 SDMMC example

Uses the SDMMC peripheral on ESP32 to retrieve from SQLite databases.

# Changes from the original   
- Supports ESP32-S3   
- Supports Long file mame   
- Supports 1Line bus width   

# Installation
```
git clone https://github.com/nopnop2002/esp32-idf-sqlite3
cd esp32-idf-sqlite3/sd_mmc
idf.py menuconfig
idf.py flash monitor
```

# Hardware requirements   
SD card reader with MMC connection.

# Configuration for ESP32
On ESP32, SDMMC peripheral is connected to specific GPIO pins using the IO MUX.   
__GPIO pins cannot be customized.__   
GPIO2 and GPIO12 cannot be changed.   
Since these GPIOs are BootStrap, it is very tricky to use 4-line SD mode with ESP32.   
I used AiThinker ESP32-CAM.   
Click [here](https://github.com/espressif/esp-idf/tree/master/examples/storage/sd_card/sdmmc) for details.

|ESP32 pin|SD card pin|Notes|
|:-:|:-:|:--|
|GPIO14|CLK|10k pullup|
|GPIO15|CMD|10k pullup|
|GPIO2|D0|10k pullup or connect to GPIO0|
|GPIO4|D1|not used in 1-line SD mode; 10k pullup in 4-line SD mode|
|GPIO12|D2|not used in 1-line SD mode; 10k pullup in 4-line SD mode|
|GPIO13|D3|not used in 1-line SD mode, but card's D3 pin must have a 10k pullup
|N/C|CD|not used in this project|
|N/C|WP|not used in this project|
|3.3V|VCC|Don't use 5V supply|
|GND|GND||

![config-top](https://github.com/nopnop2002/esp32-idf-sqlite3/assets/6020549/996ee316-12df-41fc-931f-b0b5a1fd9480)
![config-esp32-1](https://github.com/nopnop2002/esp32-idf-sqlite3/assets/6020549/b27dba7a-2bfb-4e6b-8d8c-41ca2d22319d)
![config-esp32-2](https://github.com/nopnop2002/esp32-idf-sqlite3/assets/6020549/10f07203-0ef7-487c-bc65-1d98305644bc)

# Configuration for ESP32-S3
On ESP32-S3, SDMMC peripheral is connected to GPIO pins using GPIO matrix.   
__This allows arbitrary GPIOs to be used to connect an SD card.__   
Click [here](https://github.com/espressif/esp-idf/tree/master/examples/storage/sd_card/sdmmc) for details.   
The default settings are as follows. But you can change it.   

![config-top](https://github.com/nopnop2002/esp32-idf-sqlite3/assets/6020549/6c3f7512-09d1-4975-99c9-fda1117d36e8)
![config-esp32s3-1](https://github.com/nopnop2002/esp32-idf-sqlite3/assets/6020549/8f24699c-c9b3-410f-a8ab-4b64092c12cd)
![config-esp32s3-2](https://github.com/nopnop2002/esp32-idf-sqlite3/assets/6020549/ea2d5a41-3fb0-413f-96f2-6f95fc9004df)

