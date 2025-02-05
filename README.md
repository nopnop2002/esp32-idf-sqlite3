# esp32-idf-sqlite3
sqlite3 for esp-idf.   
The original is [here](https://github.com/siara-cc/esp32-idf-sqlite3).   
Although it is a very nice library, it cannot be built with ESP-IDF Ver5.   
Therefore, We changed it so that it can also be used with ESP-IDF Ver5.   
At the same time, we also changed the sample code so that it can be used with ESP-IDF Ver5.   

# Software requirements   
ESP-IDF V5.0 or later.   
ESP-IDF V4.4 release branch reached EOL in July 2024.   

# Changes from the original   
- Change to local component   
- Disable compile warnings   
- Supports PSRAM   
- Added fatfs/littlefs example   
- Added tcp/websocket/mqtt/tusb example   
- Added query/redirect/ftp example   
- Moved sqllib.c and sqllib.h to components

# Installation
```
git clone https://github.com/nopnop2002/esp32-idf-sqlite3
cd esp32-idf-sqlite3/spiffs
idf.py menuconfig
idf.py flash monitor
```

# Using PSRAM   
Enable PARAM using Menuconfig.   
![psram-esp32](https://github.com/nopnop2002/esp32-idf-sqlite3/assets/6020549/603cd530-7424-4da6-b395-e0be6de1d6a8)


ESP32-S3 can now use two types: Quad Mode PSRAM and Octal Mode PSRAM.    
Please check the ESP32-S3 data sheet to determine which type of PSRAM your ESP32S3 has.   
![psram-esp32s3-Quad](https://github.com/nopnop2002/esp32-idf-sqlite3/assets/6020549/ed0533e0-0128-46e1-9ce1-7ffe3515b386)
![psram-esp32s3-Octal](https://github.com/nopnop2002/esp32-idf-sqlite3/assets/6020549/2ca3cfda-7128-49b9-be2d-bd7c9127488c)


sqlite uses CONFIG_SPIRAM to determine whether PSRAM is enabled or disabled.   
If PSRAM is enabled, use ps_malloc()/ps_realloc() instead of malloc()/realloc().   
```
#if CONFIG_SPIRAM
#define SQLITE_MALLOC(x) ps_malloc(x)
#define SQLITE_FREE(x) free(x)
#define SQLITE_REALLOC(x,y) ps_realloc((x),(y))
#else
#define SQLITE_MALLOC(x) malloc (x)
#define SQLITE_FREE(x) free(x)
#define SQLITE_REALLOC(x,y) realloc((x),(y))
#endif
```


If PSRAM is enabled, it will be automatically mounted at boot time.   

- ESP32-CAM
```
I (224) esp_psram: Found 8MB PSRAM device
I (224) esp_psram: Speed: 40MHz
I (228) esp_psram: PSRAM initialized, cache is in low/high (2-core) mode.
W (235) esp_psram: Virtual address not enough for PSRAM, map as much as we can. 4MB is mapped
```

- ESP32-S3-WROOM-1 N4R2/N8R2/N16R2(Quad Mode)
```
I (172) esp_psram: Found 2MB PSRAM device
I (172) esp_psram: Speed: 40MHz
```

- ESP32-S3-WROOM-1 N4R8/N8R8/N8R8/N16R8(Octal Mode)
```
I (225) esp_psram: Found 8MB PSRAM device
I (230) esp_psram: Speed: 40MHz
```


# How to use this component in your project   
Create idf_component.yml in the same directory as main.c.   
```
YourProject --+-- CMakeLists.txt
              +-- main --+-- main.c
                         +-- CMakeLists.txt
                         +-- idf_component.yml
```

Contents of idf_component.yml.
```
dependencies:
  nopnop2002/sqlite3:
    path: components/esp32-idf-sqlite3/
    git: https://github.com/nopnop2002/esp32-idf-sqlite3.git
```

When you build a projects esp-idf will automaticly fetch repository to managed_components dir and link with your code.   
```
YourProject --+-- CMakeLists.txt
              +-- main --+-- main.c
                         +-- CMakeLists.txt
                         +-- idf_component.yml
              +-- managed_components ----- nopnop2002__sqlite3
```
