# Sqlite3 TCP example
This is a demo of using sqlite3 over TCP.   
This project requires a WiFi connection.   

# Changes from the original   
- Newly created   

# Installation
```
git clone https://github.com/nopnop2002/esp32-idf-sqlite3
cd esp32-idf-sqlite3/tcp
idf.py menuconfig
idf.py flash monitor
```

# Configuration
![Image](https://github.com/user-attachments/assets/9b703004-7522-4624-8ae8-373f7c0544dd)
![Image](https://github.com/user-attachments/assets/c7fcefc3-dc7e-4ea2-ac7e-4481e3763e4f)


# Partition size
To support the ESP32-C series, this project uses a partition of 0xD0000 (=750K) by default.   
If your ESP32 has more than 4M Flash, you can change ```partitions.csv```.   


# How to use
This project works as a TCP-Server.   
A TCP-Client app is required.   
There are several TCP-Clients available that run on Windows.   
[Hercules](https://www.hw-group.com/software/hercules-setup-utility)   
[SocketTest](https://sourceforge.net/projects/sockettest/)   

I searched for a GUI tool that can be used with ubuntu or debian, but I couldn't find one.   


- Connect tcp server   
 Specify the IP address and port number of the ESP32.
![Image](https://github.com/user-attachments/assets/04472473-3afa-435a-8450-1636f5a7aaf9)   
![Image](https://github.com/user-attachments/assets/11b02618-96fe-40f3-8864-cee05e6bcc99)   
 Some applications allow you to connect using an mDNS hostname.
![Image](https://github.com/user-attachments/assets/3fe6b00b-3966-4e1a-b13e-062412999b16)

- Create database   
Type "CREATE TABLE test1 (id INTEGER, content);" and send.   
![Image](https://github.com/user-attachments/assets/d7c3f6c0-8858-4f1a-9c69-c4263f9eedf8)

- Get list of tables   
Type "select name from sqlite_master where type='table';" and send.   
![Image](https://github.com/user-attachments/assets/3f777ff0-3ecc-4bce-8ab9-f68e1666a2ed)

- Get table schema (structure)   
Type "select * from sqlite_master;" and send.   
![Image](https://github.com/user-attachments/assets/3af26414-7480-4332-8a59-004784e2ec2a)

- Delete database   
Type "drop table test1;" and send.   
![Image](https://github.com/user-attachments/assets/6df27a87-f668-4a7f-90c4-28899a9e1ee8)

- Insert data   
Type "INSERT INTO test1 VALUES (1, 'Hello, World from test1');" and send.   
Type "INSERT INTO test1 VALUES (2, 'Goodbye, World from test1');" and send.   
![Image](https://github.com/user-attachments/assets/254f2101-dbd3-4b09-a6ee-c13af3cef44b)
![Image](https://github.com/user-attachments/assets/8f1daba1-3cf7-473c-9e6f-936792713114)

- Fetch data   
Type "select * from test1;" and send.   
![Image](https://github.com/user-attachments/assets/849435c0-a1c3-4ad4-98da-2095771234cb)
![Image](https://github.com/user-attachments/assets/9e03a3a0-c163-46a2-893c-03e0526c5a48)

- Delete data   
Type "delete from test1 where id = 1;" and send.   
![Image](https://github.com/user-attachments/assets/305aab97-8e42-4f45-967d-ef664e0f4228)

- Error notification   
![Image](https://github.com/user-attachments/assets/ed368dc7-d996-41c0-9b9e-17dbb2fbad1c)

- Close database   
Press the Disconnect button.   
![Image](https://github.com/user-attachments/assets/ebbc75fb-2c3d-417c-a64b-244852abf9e2)
![Image](https://github.com/user-attachments/assets/a91cd953-1ef2-43d8-9130-747135390a9a)
