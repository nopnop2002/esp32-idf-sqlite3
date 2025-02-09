# Filr transfer using FTP
Demo of posting selected results to an FTP server.   
This project requires a WiFi connection and ftp server.   
This project uses [this](https://github.com/nopnop2002/esp-idf-ftpClient) component.   


This project will run this sql command periodically.   
```SELECT * FROM test order by rowid```   
Receive the results in a file.   

![Image](https://github.com/user-attachments/assets/128c6cac-ddba-4f75-8610-4dafd6f4e86a)

# Changes from the original   
- Newly created   

# Installation
```
git clone https://github.com/nopnop2002/esp32-idf-sqlite3
cd esp32-idf-sqlite3/ftp
idf.py menuconfig
idf.py flash monitor
```

# Configuration
![Image](https://github.com/user-attachments/assets/abb6c13a-dda3-486e-947a-fd3074fbccc8)
![Image](https://github.com/user-attachments/assets/2256d789-15ff-48d8-8e80-05cf342cce8b)

## WiFi Setting   
Set the information of your access point.   
![Image](https://github.com/user-attachments/assets/de08fcbe-84b2-4b5c-9acd-b431ca3cb88f)

## NTP Setting   
Set the information of your NTP server and time zone.   
![Image](https://github.com/user-attachments/assets/2fae3e8e-75a3-4ba7-a92b-8df4b8efa0fc)

## FTP server Setting   
Set the information of your FTP server.   
![Image](https://github.com/user-attachments/assets/4e27a9f4-0ae4-400d-9960-fde724d2ca5c)


The default output format is JSON.   
```
$ cat sqlite.json
[{
                "id":   "127008",
                "clientid":     "nop-127008",
                "updatetime":   "Wed Feb  5 13:14:01 2025"
        }, {
                "id":   "128008",
                "clientid":     "nop-128008",
                "updatetime":   "Wed Feb  5 13:14:11 2025"
        }, {
                "id":   "129008",
                "clientid":     "nop-129008",
                "updatetime":   "Wed Feb  5 13:14:21 2025"
        }, {
                "id":   "130008",
                "clientid":     "nop-130008",
                "updatetime":   "Wed Feb  5 13:14:31 2025"
        }, {
                "id":   "131008",
                "clientid":     "nop-131008",
                "updatetime":   "Wed Feb  5 13:14:41 2025"
        }, {
                "id":   "132008",
                "clientid":     "nop-132008",
                "updatetime":   "Wed Feb  5 13:14:51 2025"
        }, {
                "id":   "133008",
                "clientid":     "nop-133008",
                "updatetime":   "Wed Feb  5 13:15:01 2025"
        }, {
                "id":   "134008",
                "clientid":     "nop-134008",
                "updatetime":   "Wed Feb  5 13:15:11 2025"
        }, {
                "id":   "135008",
                "clientid":     "nop-135008",
                "updatetime":   "Wed Feb  5 13:15:21 2025"
        }, {
                "id":   "136008",
                "clientid":     "nop-136008",
                "updatetime":   "Wed Feb  5 13:15:31 2025"
        }]
```

You can change the file format to CSV format.   
![Image](https://github.com/user-attachments/assets/64082304-ae6d-4316-806e-8f29e9c62966)
```
$ cat sqlite.csv
"id","clientid","updatetime"
"11028","nop-11028","Wed Feb  5 12:17:23 2025"
"12028","nop-12028","Wed Feb  5 12:17:33 2025"
"13028","nop-13028","Wed Feb  5 12:17:43 2025"
"14028","nop-14028","Wed Feb  5 12:17:53 2025"
"15028","nop-15028","Wed Feb  5 12:18:03 2025"
"16028","nop-16028","Wed Feb  5 12:18:13 2025"
"17028","nop-17028","Wed Feb  5 12:18:23 2025"
"18028","nop-18028","Wed Feb  5 12:18:33 2025"
"19028","nop-19028","Wed Feb  5 12:18:43 2025"
"20028","nop-20028","Wed Feb  5 12:18:53 2025"
```
