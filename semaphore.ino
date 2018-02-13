/*
 *  This sketch set up a semaphore for Grupy RP Dojo challenges
 *  Connect to predefined wifi and act as simple webserver to control GPIO's
 *  
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
      blink(RED_PIN, 10, 1000);           
      accessPointSwitch();
      blink(GREEN_PIN, 5, 500);           
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
    accessPointSwitch();
  }
  server.handleClient();    
}

