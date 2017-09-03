/*
 *  This sketch set up a semaphore for Grupy RP Dojo challenges
 *  Connect to predefined wifi and act as simple webserver to control GPIO's
 *  The server will set a GPIO pin depending on the request
 *    http://server_ip/gpio/green will set the GPIO0 high and GPIO2 low
 *    http://server_ip/gpio/red will set the GPIO2 high ad GPIO0 low 
 *  server_ip is the IP address of the ESP8266 module, will be 
 *  printed to Serial when the module is connected.
 *  
 *  Author: Vanderlei
 *  Version: 1.1 02/09/2017 - Wifi repository array and connect with blink confirmation
 */

#include <ESP8266WiFi.h>

const int RED_PIN = 2; 
const int GREEN_PIN = 0;

//keeps the sizes of wifi repository for easy declaration
const int COLS = 2;
const int ROWS = 3;

//Wifi SSID and PASSWORD repository
const char* WIFI_REPO[COLS][ROWS] = {
  {"SSID_1", "SSID_2", "SSID_3"},
  {"PASS_1", "PASS_2","PASS_3"}
};

//List networks for debugging pourposes
void listNetworks() {
  // scan for nearby networks:
  Serial.println("** Scan Networks **");
  int numSsid = WiFi.scanNetworks();
  if (numSsid == -1)
  {
    Serial.println("Couldn't get a wifi connection");
    while (true);
  }
  // print the list of networks seen:
  Serial.print("number of available networks:");
  Serial.println(numSsid);

  // print the network number and name for each network found:
  for (int thisNet = 0; thisNet < numSsid; thisNet++) {
    Serial.print(thisNet);
    Serial.print(") ");
    Serial.print(WiFi.SSID(thisNet));
    Serial.print("\tSignal: ");    
    Serial.print(WiFi.RSSI(thisNet));
    Serial.print(" dBm");
    Serial.println();
    Serial.flush();
  }
}

//Connect to wifi that first matches one of the WIFI_REPO and print status to serial out
//Returns true if wifi is sucessfuly connected, else returns false
bool connectWifi(){  
  for (int i = 0; i <= ROWS -1; i++){        
    Serial.println();
    Serial.println();
    Serial.print("Connecting to ");
    Serial.println(WIFI_REPO[0][i]);
    
    WiFi.begin(WIFI_REPO[0][i], WIFI_REPO[1][i]);     
    
    int count = 0;
    while (WiFi.status() != WL_CONNECTED) {
      if (count >= 80){ //wait about 40 seconds to connect i-th wifi on list 
        Serial.println();
        Serial.print(WIFI_REPO[0][i]);
        Serial.print(" Not reached...");
        break;
      }
      
      digitalWrite(RED_PIN, HIGH);
      delay(250);
      Serial.print(".");
      digitalWrite(RED_PIN, LOW);
      delay(250);
      count += 1;
    }
    if (WiFi.status() == WL_CONNECTED){ //Stop loop      
      Serial.println("");
      Serial.println("WiFi connected");
      return true;     
    }    
  }   
  return false;  
}

// Change state of pins and do a blink warning
//First parameter stays OFF and second ON after blinking 
void changeState(int pinX, int pinY){  
  digitalWrite(pinX, LOW);
  digitalWrite(pinY, HIGH);
  delay(200);
  for (int i = 0; i <= 3; i++){      
    digitalWrite(pinY, LOW);
    delay(200);
    digitalWrite(pinY, HIGH);    
    delay(200);
  }      
}

//high and low the pin for the number of times specified in loops and
//the timing in customDelay
void blink(int pin, int loops, int customDelay){
  for (int i = 0; i <= loops; i++){
    digitalWrite(pin, HIGH);
    delay(customDelay);
    digitalWrite(pin, LOW);
    delay(customDelay);
  }
}

// Create an instance of the server
// specify the port to listen on as an argument
WiFiServer server(80);

void setup() {
  Serial.begin(9600);
  delay(10);

  // prepare GPIO's
  pinMode(GREEN_PIN, OUTPUT);
  digitalWrite(GREEN_PIN, LOW);
  pinMode(RED_PIN, OUTPUT);
  digitalWrite(RED_PIN, LOW);

  //List available networks. Only for debugging
  listNetworks();
  
  // Connect to WiFi network
  bool isConnected = connectWifi();
  
  // Start the server
  if (isConnected){
    server.begin();
    Serial.println("Server started");

    //blink indicating wifi has connected and server is UP
    //pin, loop length, delay time in miliseconds
    blink(GREEN_PIN, 20, 50);

    // Print the IP address
    Serial.println(WiFi.localIP()); 
   }
   else{
    while(true)      
      blink(RED_PIN, 25, 1000);      
   }
}

void loop() {
  // Check if a client has connected
  WiFiClient client = server.available();
  if (!client) {
    return;
  }
  
  // Wait until the client sends some data
  Serial.println("new client");
  while(!client.available()){
    delay(1);
  }
  
  // Read the first line of the request
  String req = client.readStringUntil('\r');
  Serial.println(req);
  client.flush();
  
  // Match the request
  int val;
  if (req.indexOf("/gpio/green") != -1)
    val = 0;
  else if (req.indexOf("/gpio/red") != -1)
    val = 1;
  else if (req.indexOf("/gpio/off") != -1)
    val = 2;
  else {
    Serial.println("invalid request");
    client.stop();
    return;
  }

  // Set GPIO's according to the request  
  if (val == 1)
    changeState(GREEN_PIN, RED_PIN);  
  else if (val == 0)
    changeState(RED_PIN, GREEN_PIN);      
  else if (val == 2){
    digitalWrite(GREEN_PIN, LOW);
    digitalWrite(RED_PIN, LOW);
  }
   
  client.flush();

  String bgGreen = "<h1><span style='background-color:green;'>GREEN</span></h1>";
  String bgRed = "<h1><span style='background-color:red;'>RED</span></h1>";
  String bgOff = "<h1><span style='background-color:yellow;'>OFF</span></h1>";

  // Prepare the response
  String s = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n<!DOCTYPE HTML>\r\n<html>\r\nSEMAPHORE is now ";
  //s += (val)?"GREEN":"RED";
  if (val == 0)
    s += bgGreen;
  else if(val == 1)
    s += bgRed;
  else
    s += bgOff;    
  //s += (val)?bgGreen:bgRed;
  s += "</html>\n";

  // Send the response to the client
  client.print(s);
  delay(1);
  Serial.println("Client disconnected");

  // The client will actually be disconnected 
  // when the function returns and 'client' object is detroyed   
}
