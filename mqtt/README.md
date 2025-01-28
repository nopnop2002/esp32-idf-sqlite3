# Sqlite3 Over MQTT
Demo of using sqlite3 over MQTT.   
This project requires a WiFi connection.   

# Changes from the original   
- Newly created   

# Installation
```
git clone https://github.com/nopnop2002/esp32-idf-sqlite3
cd esp32-idf-sqlite3/mqtt
idf.py menuconfig
idf.py flash monitor
```

# Configuration
![Image](https://github.com/user-attachments/assets/78413f16-1ef2-4233-be32-0b11e12f2a93)
![Image](https://github.com/user-attachments/assets/d343837a-d2f5-469a-9012-ddd8d5031b0b)

## WiFi Setting   
![Image](https://github.com/user-attachments/assets/80e5d873-bbd4-4e5d-a710-678bc1b52cc0)

## Broker Setting   
![Image](https://github.com/user-attachments/assets/11b96d46-7eb3-49de-a0c1-1d1ba927aa0d)

### Select Transport   
This project supports TCP,SSL/TLS,WebSocket and WebSocket Secure Port.   

- Using TCP Port.   
 TCP Port uses the MQTT protocol.   

- Using SSL/TLS Port.   
 SSL/TLS Port uses the MQTTS protocol instead of the MQTT protocol.   

- Using WebSocket Port.   
 WebSocket Port uses the WS protocol instead of the MQTT protocol.   

- Using WebSocket Secure Port.   
 WebSocket Secure Port uses the WSS protocol instead of the MQTT protocol.   

__Note for using secure port.__   
The default MQTT server is ```broker.emqx.io```.   
If you use a different server, you will need to modify ```getpem.sh``` to run.   
```
chmod 777 getpem.sh
./getpem.sh
```

### Select MQTT Protocol   
This project supports MQTT Protocol V3.1.1/V5.   
![Image](https://github.com/user-attachments/assets/2217cd2d-db3b-4ecf-8ba2-cefb3209e279)

### Enable Secure Option   
Specifies the username and password if the server requires a password when connecting.   
[Here's](https://www.digitalocean.com/community/tutorials/how-to-install-and-secure-the-mosquitto-mqtt-messaging-broker-on-debian-10) how to install and secure the Mosquitto MQTT messaging broker on Debian 10.   
![Image](https://github.com/user-attachments/assets/cd0115f4-d15b-42a6-aaa6-e7c1e76d18cc)

# Partition size
To support the ESP32-C series, this project uses a partition of 0xB0000 (=690K) by default.   
If your ESP32 has more than 4M Flash, you can change ```partitions.csv```.   

# How to use
A MQTT-Client app is required.   
There are several MQTT-Clients available that run on Windows.   
I used [this](https://mqttx.app/) application.   
There are many other applications available.   

- Create table   
Type "CREATE TABLE test1 (id INTEGER, content);" and send.   
![Image](https://github.com/user-attachments/assets/0443e49a-ef2f-4bb0-ac50-fe76b6d6e275)

- Get list of tables   
Type "select name from sqlite_master where type='table';" and send.   
![Image](https://github.com/user-attachments/assets/91498fab-491c-46a3-b344-bc19eb462f69)

- Get table schema (structure)   
Type "select * from sqlite_master;" and send.   
![Image](https://github.com/user-attachments/assets/b2071bc5-9d9e-4831-808f-7545127cf67d)

- Delete table   
Type "drop table test1;" and send.   
![Image](https://github.com/user-attachments/assets/e0cf3bd6-ad14-449d-a400-2355487376d4)

- Insert data   
Type "INSERT INTO test1 VALUES (1, 'Hello, World from test1');" and send.   
Type "INSERT INTO test1 VALUES (2, 'Goodbye, World from test1');" and send.   
![Image](https://github.com/user-attachments/assets/720d7d31-92eb-4605-a18e-bac27e71d5a1)
![Image](https://github.com/user-attachments/assets/c2691d42-ce9f-4eb9-a51e-aa90b3e8ff79)

- Fetch data   
Type "select * from test1;" and send.   
![Image](https://github.com/user-attachments/assets/3a16bdd6-f956-4c4b-b74f-98be7640b734)

- Delete data   
Type "delete from test1 where id = 1;" and send.   
![Image](https://github.com/user-attachments/assets/e97e42cd-e61f-40af-8841-2249cb58cb92)

- Error notification   
![Image](https://github.com/user-attachments/assets/cb97ec84-ff0a-4a4f-bdf7-de0ead30bc10)


