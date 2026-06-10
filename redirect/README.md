# Redirect to network
Demo of redirecting select results to the network.   
You can use UDP/MQTT/HTTP for the protocol.   
This project requires a WiFi connection.   

This project executes this SQL command at startup.   
The datetime column will be set to the current date and time.   
```
CREATE TABLE test (id INTEGER, datetime);
INSERT INTO test VALUES (1, '%s');
```

This project will execute this SQL command periodically.   
The datetime column will be updated with the current date and time.   
SELECT results are received via UDP/MQTT/HTTP.   
```
UPDATE test SET datetime = '%s' where id = 1;
SELECT datetime FROM test where id = 1;
```

<img width="1280" height="720" alt="Image" src="https://github.com/user-attachments/assets/89b682a7-1658-4277-8559-4e5b81850cad" />

# Changes from the original   
- Newly created   

# Installation
```
git clone https://github.com/nopnop2002/esp32-idf-sqlite3
cd esp32-idf-sqlite3/redirect
idf.py menuconfig
idf.py flash monitor
```

# Configuration
![Image](https://github.com/user-attachments/assets/ead9a091-8293-4b70-ac59-a173d05751e5)
![Image](https://github.com/user-attachments/assets/a7b1265d-332e-412f-ab58-4ca04a0e2ac0)

## WiFi Setting   
Set the information of your access point.   
![Image](https://github.com/user-attachments/assets/6a382a6e-e687-4651-a210-801b29b69ea8)

## NTP Setting   
Set the information of your NTP server and time zone.   
![Image](https://github.com/user-attachments/assets/10f9748b-5987-4c47-894f-b902c692b9a5)

## Redirect Setting   
Set the information of network protocol.   
![Image](https://github.com/user-attachments/assets/983b2485-072b-4f9d-a3a2-cbb8c65ffca9)


### Redirect to UDP
Set the information of your UDP listner.   
![Image](https://github.com/user-attachments/assets/17f16f27-7c49-4041-b48d-97cf1da9dcaf)

ESP32 works as a UDP client.   
Requires a UDP server.   
I used [this](http://xn--sourceforge-m061am75l.net/projects/sockettest/) application.   
![Image](https://github.com/user-attachments/assets/34af570b-ebfc-460e-a0f5-f187e2278053)

We can use netcat as a UDP server.   
```
while true;do nc -ul -p 8080 -w 0; echo ''; done
```
<img width="659" height="486" alt="Image" src="https://github.com/user-attachments/assets/e346c141-80a2-46df-a5d9-0365823c3334" />

### Redirect to MQTT
Set the information of your MQTT broker.   
![Image](https://github.com/user-attachments/assets/482bded1-59c5-4951-b624-770612595516)

ESP32 works as a MQTT Publisher.   
Requires a MQTT Subscriber.   
I used [this](https://mqttx.app/) application.   
![Image](https://github.com/user-attachments/assets/002c6981-75f0-4ac1-a7de-9a156e4261e1)

We can use mosquitto client.   
![Image](https://github.com/user-attachments/assets/ed10714a-6f91-4a1a-aa7a-51b4893de276)


### Redirect to HTTP
Set the information of your HTTP server.   
<img width="659" height="486" alt="Image" src="https://github.com/user-attachments/assets/9d91e5a4-88b9-4a1d-b6a4-a9a2c52873bc" />

ESP32 works as a HTTP client.   
We can use `http.server.py`.   
<img width="659" height="486" alt="Image" src="https://github.com/user-attachments/assets/910baf40-2adc-44b2-bb8f-4edae9368b97" />

We can use netcat as a HTTP server.   
```
while true; do printf "HTTP/1.1 200 OK\r\n\r\n" | nc -l -p 8080; done
```
<img width="659" height="486" alt="Image" src="https://github.com/user-attachments/assets/a92c3fd6-109a-4dd9-b5ce-88f0d43da87a" />

