/*
 *  This sketch set up a semaphore for Grupy RP Dojo challenges
 *  Connect to predefined wifi and act as simple webserver to control GPIO's
 *  The server will set a GPIO pin depending on the request
 *    http://server_ip_or_hostname/set?green=on will set the GPIO0 high and GPIO2 low
 *    http://server_ip_or_hostname/set?red=on will set the GPIO2 high and GPIO0 low
 *  server_ip is the IP address of the ESP8266 module, will be 
 *  printed to Serial when the module is connected.
 *  
 *  Author: Vanderlei
 *  Version: 2.0
 *    v 2.0 12/02/2018 - All code refactored, new functions added. See readme.md
 *  Version: 1.2
 *    v 1.2 28/10/2017 - Log IP address to dweet.io at https://dweet.io/follow/grupyespsemaphore  
 *      and split in two .ino files (semaforo, auxiliar)
 *    v 1.1 31/08/2017 - Wifi array repository and connect blink confirmation *  
 */

 extern "C"{
  #include "user_interface.h"
 }

#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ArduinoJson.h>
#include "FS.h"

const int RED_PIN = 2; 
const int GREEN_PIN = 0;

char* espHostname = "semaforo-grupy";

unsigned long prevMillis = 0;
int interval = 3600000;//60 minutes

// Create an instance of the server
// specify the port to listen on as an argument
ESP8266WebServer server(80);

void setup() {
  Serial.begin(115200);
  delay(10);

  // prepare GPIO's
  pinMode(GREEN_PIN, OUTPUT);
  digitalWrite(GREEN_PIN, LOW);
  pinMode(RED_PIN, OUTPUT);
  digitalWrite(RED_PIN, LOW);  

  WiFi.hostname(espHostname);   

  //List available networks. Only for debugging
  listNetworks();

  Serial.println(WiFi.SSID());
  
  //Only station mode - Default is Station and AP
  WiFi.mode(WIFI_STA);    
  
  // Connect to WiFi network
  bool isConnected = newConnectWifi();
  
  // Start the server
  if (isConnected){    
    //blink indicating wifi has connected and server is UP
    //pin, loop length, delay time in miliseconds
    blink(GREEN_PIN, 20, 50);     

    //Webserver
    startServer();

    // Print the IP address
    Serial.println(WiFi.localIP());

    Serial.print("Host name: ");
    Serial.println(WiFi.hostname());

    // Post IP to dweet.io
    postIPToDweet(WiFi.localIP());

    Serial.println("");
    Serial.println("Debug:");   
    WiFi.printDiag(Serial);
   }
   else{
    //while(true)   
      Serial.println("");
      Serial.println("Debug:");   
      WiFi.printDiag(Serial);
      blink(RED_PIN, 25, 1000);      
   }
}

void loop() {  

  /*Check if wifi is connected
     If not, turn on AP mode
  */
  unsigned long currentMillis = millis();
  if ((unsigned long)(currentMillis - prevMillis) >= interval ) {
    prevMillis = currentMillis;   

    //If there's no connection, turn AP mode on
    //to set wifi client SSID and password
    if (WiFi.status() != WL_CONNECTED) {
      delay(100);
      WiFi.softAP("Esp_Access_Point");
      WiFi.mode(WIFI_AP_STA);      
    }
  }

  server.handleClient();
    
}

