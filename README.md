# ESP-01 SEMAPHORE

## ESP-01 Wifi Semaphore

Red light = GPIO2  
Green light = GPIO0  

Usage:  
* change your ESP-01 name in auxiliar.ino on **dweetThingName** constant  
* create **_assids.ino_** file on same arduino project folder with following model:  
	```c	
        const int COLS = 2;  
        const int ROWS = 3;  
        const char* WIFI_REPO[COLS][ROWS] = {  
        	{"SSID1",     "SSID2",     "SSID3"},  
        	{"PASSWORD1", "PASSWORD2", "PASSWORD3"}  
        };          	
	```    
* change SSID's data with your own **SSID**s  
* Compile skecth and upload to ESP-01
* Upload Data folder to ESP-01. If plugin is not instaled on Arduino IDE, install it in https://github.com/esp8266/arduino-esp8266fs-plugin
* Get your ESP-01 IP address: https://dweet.io/follow/your_dweetThingName or use hostname provided for ESP8266WEBSERVER library
* Interact with your ESP-01: *http://your_ip_address_or_hostname*   
![ESP Semaphore](https://image.ibb.co/bPsQfn/espsemaphore.png "ESP Semaphore")
* GET requests returning Json: *http://your_ip_address_or_hostname/set?**green=on*** or **_red=on_**  
* Request status: *http://your_ip_address_or_hostname/status*
