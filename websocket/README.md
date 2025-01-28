# Sqlite3 Over WebSocket
Demo of using sqlite3 over WebSocket.   
This project requires a WiFi connection.   

![Image](https://github.com/user-attachments/assets/dff2d26f-fc94-42e8-b56e-19f72b59402e)

# Changes from the original   
- Newly created   

# Installation
```
git clone https://github.com/nopnop2002/esp32-idf-sqlite3
cd esp32-idf-sqlite3/websocket
idf.py menuconfig
idf.py flash monitor
```

# Configuration
![Image](https://github.com/user-attachments/assets/f40aa008-2da6-4756-9be0-1fa4a3ef8cde)
![Image](https://github.com/user-attachments/assets/b173642c-2716-431f-817f-901633024f58)


# Partition size
To support the ESP32-C series, this project uses a partition of 0xD0000 (=750K) by default.   
If your ESP32 has more than 4M Flash, you can change ```partitions.csv```.   


# How to use
This project works as a WebSocket-Server.   
Open a browser and enter the ESP32 IP address in the address bar.   
You can use ```esp32.local``` instead of the IP address.   
![Image](https://github.com/user-attachments/assets/e0d314a5-f5ae-4f61-b187-1dad0ed2602f)

- Create table   
Type "CREATE TABLE test1 (id INTEGER, content);" and send.   
![Image](https://github.com/user-attachments/assets/5ae7102f-90f0-4712-9af0-65ac820ab4d4)

- Get list of tables   
Type "select name from sqlite_master where type='table';" and send.   
![Image](https://github.com/user-attachments/assets/87f3bc38-a3d2-41bc-a4d8-16baec02da72)

- Get table schema (structure)   
Type "select * from sqlite_master;" and send.   
![Image](https://github.com/user-attachments/assets/5d8e67ed-2529-4521-b69f-0a6aba227e3e)

- Delete table   
Type "drop table test1;" and send.   
![Image](https://github.com/user-attachments/assets/a50bbe7e-7ca8-4869-b751-4d292e62cdff)

- Insert data   
Type "INSERT INTO test1 VALUES (1, 'Hello, World from test1');" and send.   
Type "INSERT INTO test1 VALUES (2, 'Goodbye, World from test1');" and send.   
![Image](https://github.com/user-attachments/assets/56af248a-ef61-4d7f-acbb-59065e9f1c24)
![Image](https://github.com/user-attachments/assets/802d9151-6a80-4ae2-8b15-74e9ca3bff8e)

- Fetch data   
Type "select * from test1;" and send.   
![Image](https://github.com/user-attachments/assets/c3b98348-dce6-4d0c-8ad6-21bfedf9e1d6)

- Delete data   
Type "delete from test1 where id = 1;" and send.   
![Image](https://github.com/user-attachments/assets/8c501f3a-318a-4ae5-a0a0-ba2100e05905)

- Error notification   
![Image](https://github.com/user-attachments/assets/498b307b-d0d8-4cab-8913-e0dd0e7e1520)


