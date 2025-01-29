# Sqlite3 Over TinyUSB
Demo of using sqlite3 over TinyUSB.   
This project requires a USB connection.   

![Image](https://github.com/user-attachments/assets/0a9e0d27-a8f7-42e1-bd5b-99efed1c2648)


# Hardware requirements
1. ESP32-S2/S3 Development board   
Because the ESP32-S2/S3 does support USB OTG.   

2. USB Connector   
I used this:   
![usb-connector](https://user-images.githubusercontent.com/6020549/124848149-3714ba00-dfd7-11eb-8344-8b120790c5c5.JPG)

```
ESP32-S2/S3 BOARD          USB CONNECTOR
                           +--+
                           | || VCC
    [GPIO 19]    --------> | || D-
    [GPIO 20]    --------> | || D+
    [  GND  ]    --------> | || GND
                           +--+
```

# Configuration
![Image](https://github.com/user-attachments/assets/8f602034-9084-4416-883a-3259a9eca52b)
![Image](https://github.com/user-attachments/assets/be08676f-f2d7-4324-9ead-26fe2c28e991)

# Using Windows Terminal Software
When you connect the USB cable to the USB port on your Windows machine and build the firmware, a new COM port will appear.   
Open a new COM port in the terminal software.   
I used TeraTerm.   
__Since it's not bash, you can't use the arrow keys.__   
![Image](https://github.com/user-attachments/assets/0a9e0d27-a8f7-42e1-bd5b-99efed1c2648)

# Using Linux Terminal Software
When you connect the USB cable to the USB port on your Linux machine and build the firmware, a new /dev/ttyACM0 device will appear.   
Open a new tty device in the terminal software.   
I used cu command.   
__Since it's not bash, you can't use the arrow keys.__   
![Image](https://github.com/user-attachments/assets/5fa0a385-48e6-40fe-b14d-14cee5e2f387)




