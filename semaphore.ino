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
 *  Version: 1.2
 *    v 1.2 28/10/2017 - Log IP address to dweet.io at https://dweet.io/follow/grupyespsemaphore  
 *      and split in two .ino files (semaforo, auxiliar)
 *    v 1.1 31/08/2017 - Wifi array repository and connect blink confirmation *  
 */

#include <ESP8266WiFi.h>

const int RED_PIN = 2; 
const int GREEN_PIN = 0;

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
  
  //Only station mode - Default is Station and AP
  WiFi.mode(WIFI_STA);
  
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

    // Post IP to dweet.io
    postIPToDweet(WiFi.localIP());
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
  int timeout = 0;
  Serial.println("new client");
  while(!client.available()){
    //Serial.print(".");
    delay(1);
    timeout++;
    if(timeout > 10000){
      Serial.print("Infinite loop break!");
      break;
    }
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
  client.stop();
}

