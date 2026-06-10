# Sqlite3 Over UDP
Demo of using sqlite3 over UDP.   
This project requires a WiFi connection.   
You can execure SQL commands using udp software.   

<img width="586" height="493" alt="Image" src="https://github.com/user-attachments/assets/dab4715e-d0eb-4530-bc95-f8d4866170b0" />

# Changes from the original   
- Newly created   

# Installation
```
git clone https://github.com/nopnop2002/esp32-idf-sqlite3
cd esp32-idf-sqlite3/udp
idf.py menuconfig
idf.py flash monitor
```

# Configuration
<img width="659" height="486" alt="Image" src="https://github.com/user-attachments/assets/96ce7899-644d-47a1-8b0c-4aefa4be6ecc" />

Set the information of your access point.   
<img width="659" height="486" alt="Image" src="https://github.com/user-attachments/assets/c7381b72-0d1c-4ff4-beb9-276685786904" />

# Partition size
To support the ESP32-C series, this project uses a partition of 0xD0000 (=750K) by default.   
If your ESP32 has more than 4M Flash, you can change ```partitions.csv```.   


# How to use
This project works as a UDP-Server.   
A UDP-Client app is required.   
There are several UDP-Clients available that run on Windows.   
[Hercules](https://www.hw-group.com/software/hercules-setup-utility)   
[SocketTest](https://sourceforge.net/projects/sockettest/)   

I searched for a GUI tool that can be used with ubuntu or debian, but I couldn't find one.   


- Connect udp server   
	Specify the IP address and port number of the ESP32.   
	<img width="586" height="493" alt="Image" src="https://github.com/user-attachments/assets/86c369e6-9ab0-4e83-b88d-2c6ca9cd3a9c" />   
	Some applications allow you to connect using an mDNS hostname.   
	<img width="586" height="493" alt="Image" src="https://github.com/user-attachments/assets/4c9e9fe1-b6d7-49bf-8302-0da9f196b2a7" />

- Create table   
	Type "CREATE TABLE test1 (id INTEGER, content);" and send.   
	<img width="586" height="493" alt="Image" src="https://github.com/user-attachments/assets/6da8b74f-9348-4f0d-995f-8af18a757f03" />

- Get list of tables   
	Type "select name from sqlite_master where type='table';" and send.   
	<img width="586" height="493" alt="Image" src="https://github.com/user-attachments/assets/93a3329e-3ec5-491a-87b2-678f54a58ea2" />

- Get table schema (structure)   
Type "select * from sqlite_master;" and send.   
<img width="586" height="493" alt="Image" src="https://github.com/user-attachments/assets/ab1b60c3-0220-413f-b909-ad5fc5827125" />

- Insert data   
Type "INSERT INTO test1 VALUES (1, 'Hello, World from test1');" and send.   
Type "INSERT INTO test1 VALUES (2, 'Goodbye, World from test1');" and send.   
<img width="586" height="493" alt="Image" src="https://github.com/user-attachments/assets/1b663b7c-33f4-40ea-9ea8-5320b1c8a2ad" />
<img width="586" height="493" alt="Image" src="https://github.com/user-attachments/assets/79490a6f-93a7-473f-8cb7-a33b61ddebab" />

- Fetch data   
Type "select * from test1;" and send.   
<img width="586" height="493" alt="Image" src="https://github.com/user-attachments/assets/dab4715e-d0eb-4530-bc95-f8d4866170b0" />

- Delete data   
Type "delete from test1 where id = 1;" and send.   
<img width="586" height="493" alt="Image" src="https://github.com/user-attachments/assets/cf4bcb0a-6b32-40f8-a80f-9d2167c69273" />

- Delete table   
Type "drop table test1;" and send.   
<img width="586" height="493" alt="Image" src="https://github.com/user-attachments/assets/d19d01ed-c988-43e2-be0f-47afc31a32d8" />

- Error notification   
<img width="586" height="493" alt="Image" src="https://github.com/user-attachments/assets/8cc267dc-3591-4b6c-9360-2b8ec462433b" />


# Using netcat   
We can use netcat as UDP client.   
<img width="659" height="486" alt="Image" src="https://github.com/user-attachments/assets/d4ae42b7-f929-42df-a224-6fb550b5435f" />

I used sed to format the output.   
```
echo "select * from test1;" | nc -u 192.168.0.131 9876 -w 1 | sed -e 's/id/\nid/g' | sed -e 's/content/ content/g'; echo
```
<img width="659" height="486" alt="Image" src="https://github.com/user-attachments/assets/e159a066-cb64-4d46-96d7-e1e790fa2b75" />

