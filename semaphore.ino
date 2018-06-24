/*
    This sketch sets up a semaphore for Grupy RP Dojo challenges,
    Connects to predefined wifi and acts as a simple webserver to 
    control GPIO's

*/

extern "C" {
#include "user_interface.h"
}

#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <DNSServer.h>
#include <WiFiManager.h>
#include <ArduinoJson.h>
#include "FS.h"

const int RED_PIN = 2;
const int GREEN_PIN = 0;

char* espHostname = "semaforo-grupy";

//Millis variables for WiFi Checking
unsigned long prevMillis = 0;
int interval = 32000;//32 segundos
byte retryCount = 0;

////Millis variables for WiFi status blink
//unsigned long statusPrevMillis = 0;
//int statusInterval = 20000;//20 seconds
//byte blinkBreak = 5; 

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

  //Set hostname for verbose access: http://esp_hostname.home
  WiFi.hostname(espHostname);

  String stSSID = getStoredCredentials("ssid");
  String stPSK = getStoredCredentials("password");

  //List available networks and return true if the current SSID matches
  bool ssidMatch = listNetworks(stSSID);   
  Serial.println("SSID reached: " + String(ssidMatch));
  
  //Start wifi with last saved credentials
  WiFi.begin(stSSID.c_str(), stPSK.c_str());  

  while (WiFi.status() != WL_CONNECTED) {
    delay(10);
    unsigned long currentMillis = millis();
    if ((unsigned long)(currentMillis - prevMillis) >= interval ) {
      prevMillis = currentMillis;
      Serial.println("More than 30 seconds passed. Turning AP Mode ON...");
      break;    
    }
    
    if (WiFi.status() == WL_CONNECTED) { //Stop loop
      Serial.println("");
      Serial.println("*** WiFi connected ***");
      blink(GREEN_PIN, 20, 50);
    }
  } 
  
  //Only station mode - Default is Station and AP
  WiFi.mode(WIFI_STA);

  //If WiFi connection doesn't starts, wifiManager runs a web server 
  //to set new credentials
  WiFiManager wifiManager;
  wifiManager.autoConnect("[Grupy-RP AP]");

  if(WiFi.status() == WL_CONNECTED){
    if(WiFi.SSID() != stSSID || WiFi.psk() != stPSK ){
      setWifiCredentials(WiFi.SSID(), WiFi.psk());
      Serial.println("Newest credentials stored");  
    }    
    
    Serial.print("ESP hostname: ");
    Serial.println(WiFi.hostname());

    // Post IP to dweet.io
    postIPToDweet(WiFi.localIP());
  }

  //start semaphore webserver
  startServer();

  //Reuse previous millis and interval variables
  prevMillis = 0;
  interval = 10 * (1000 * 60); //10 minutes  
  //interval = 1 * (1000 * 60); //1 minute debug interval
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
    Serial.println("+++++ Millis executed +++++");
    
    if(WiFi.status() != WL_CONNECTED){
      WiFi.begin();      
      retryCount += 1;
      Serial.println("Wifi begin. Try number " + String(retryCount));
    }
    else{
      retryCount = 0;      
      postIPToDweet(WiFi.localIP());
    }

    //PIECE OF CODE NOT WORKING - IT NEEDS TO BE REFACTORED
    //WIFIMANAGER LOCKS WHEN IS ON
    //ESP REBOOT IN FW MODE WHEN RESET OR RESTART IS EXECUTED
    if(retryCount >= 1){
      Serial.println("Trying to restart ESP...");
      WiFiManager wifiManager;
      wifiManager.autoConnect("[Grupy-RP AP]");
      retryCount = 0;
      //digitalWrite(RED_PIN, HIGH);
      //digitalWrite(GREEN_PIN, HIGH);
      //delay(2000);
      //ESP.restart();
    }
  }

  server.handleClient();
}

