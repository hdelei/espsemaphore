/*
   This sketch keeps all functions and networks
*/

//Create your own thing name
const String dweetThingName = "grupyespsemaphore";

//high and low the pin for the number of times specified in loops and
//the timing in customDelay
void blink(int pin, int loops, int customDelay) {
  for (int i = 0; i <= loops; i++) {
    digitalWrite(pin, HIGH);
    delay(customDelay);
    digitalWrite(pin, LOW);
    delay(customDelay);
  }
}

// Change state of pins and do a blink warning
//First parameter stays OFF and second ON after blinking
void changeState(int pinX, int pinY) {
  digitalWrite(pinX, LOW);
  digitalWrite(pinY, HIGH);
  delay(200);
  for (int i = 0; i <= 3; i++) {
    digitalWrite(pinY, LOW);
    delay(200);
    digitalWrite(pinY, HIGH);
    delay(200);
  }
}

//Connect to wifi that first matches one of the WIFI_REPO and print status to serial out
//Returns true if wifi is sucessfuly connected, else returns false
//bool connectWifi() {
//  //String currentSSID = WiFi.SSID();
//  //String currentPSK = WiFi.psk();
//
//  for (int i = 0; i <= ROWS - 1; i++) {
//    Serial.println();
//    Serial.println();
//    Serial.print("Connecting to ");
//    Serial.println(WIFI_REPO[0][i]);
//
//    WiFi.begin(WIFI_REPO[0][i], WIFI_REPO[1][i]);
//
//    int count = 0;
//    while (WiFi.status() != WL_CONNECTED) {
//      if (count >= 80) { //wait about 40 seconds to connect i-th wifi on list
//        Serial.println();
//        Serial.print(WIFI_REPO[0][i]);
//        Serial.print(" Not reached...");
//        break;
//      }
//
//      digitalWrite(RED_PIN, HIGH);
//      delay(250);
//      Serial.print(".");
//      digitalWrite(RED_PIN, LOW);
//      delay(250);
//      count += 1;
//    }
//    if (WiFi.status() == WL_CONNECTED) { //Stop loop
//      Serial.println("");
//      Serial.println("WiFi connected");
//      return true;
//    }
//  }
//  return false;
//}

//New Connection code improved. Load faster than connectWifi()
//
bool newConnectWifi() {

  bool match = false;
  bool hasWifi = true;
  String currentSSID = WiFi.SSID();
  int count = WiFi.scanNetworks();

  if (count == 0)
    return false;
  if (count > 0) {
    unsigned long prev = 0;
    int duration = 60000;//60 seconds
    unsigned long current;

    int matchIndex = -1;
    for (int i = 0; i <= ROWS - 1; i++) {
      for (int j = 0; j < count; j++) {
        if (String(WIFI_REPO[0][i]) == WiFi.SSID(j)) {
          matchIndex = i;
          break;
        }
        if (matchIndex > -1) break;
      }
    }

    if (matchIndex > -1) {
      int i = matchIndex;
      WiFi.begin(WIFI_REPO[0][i], WIFI_REPO[1][i]);

      Serial.print("Connecting to: ");
      Serial.println(WIFI_REPO[0][i]);

      while (WiFi.status() != WL_CONNECTED) {
        delay(10);
        if (WiFi.status() == WL_CONNECTED) { //Stop loop
          Serial.println("");
          Serial.println("WiFi connected");
          return true;
        }

        current = millis();
        if ((unsigned long)(current - prev) >= duration ) {
          prev = current;
          Serial.println("No connection");
          return false;
        }
      }
    }
  }
  return false;
}

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

bool setWifiCredentials(String ssid, String password) {
  String filename = "/config.json";

  SPIFFS.begin();
  File file;

  if (SPIFFS.exists(filename))
    file = SPIFFS.open(filename, "r");

  DynamicJsonBuffer jsonBuffer;
  JsonObject& root = jsonBuffer.parseObject(file);

  if (!root.success()) {
    Serial.println("Can't parse json file");
    return false;
  }

  if (root.containsKey("ssid") && root["ssid"] == ssid) {
    root["password"] = password;
  }
  else {
    root["ssid"] = ssid;
    root["password"] = password;
  }

  file.close();

  SPIFFS.remove(filename);
  file = SPIFFS.open(filename, "w");
  root.printTo(file);

  file.close();
  SPIFFS.end();

  printFile("config.json");//Serial debugging
  
  return true;
}

void connectWithNewCredentials(){
  String filename = "/config.json";
  
  SPIFFS.begin();
  File file;

  if (SPIFFS.exists(filename))
    file = SPIFFS.open(filename, "r");

  DynamicJsonBuffer jsonBuffer;
  JsonObject& root = jsonBuffer.parseObject(file);

  if (!root.success()) {
    Serial.println("Can't parse json file");
    return;
  }

//  String ssid;
//  String password;  

  String ssid = root["ssid"].as<String>();
  String password = root["password"].as<String>();

  file.close();
  SPIFFS.end();

  WiFi.mode(WIFI_OFF); 
  Serial.println("");
  Serial.println("Wifi off. Wait 20 seconds to reconnection.");
  delay(1000 * 10);
  WiFi.mode(WIFI_STA);    
  WiFi.begin(ssid.c_str(), password.c_str());
  Serial.println("");
  Serial.println("Trying to reconnect...");
  delay(100);
}

//Print content of given file
void printFile(String filename) {
  filename = "/" + filename;
  SPIFFS.begin();
  File file = SPIFFS.open(filename, "r");

  Serial.println("");
  Serial.println("Printing file content: ");
  while (file.available()) {
    Serial.print((char)file.read());
  }

  file.close();
  SPIFFS.end();
}

void handleIndex() {
  SPIFFS.begin();
  File index;
  String text = "Nothing read";
  if (SPIFFS.exists("/index.html")) {
    index = SPIFFS.open("/index.html", "r");
    text =  index.readString();
    index.close();
  }
  SPIFFS.end();

  sendResponse("text/html", text);
}

void handleWifiPage() {
  bool validArgs = true;
  String arg1, arg2;
  if (server.hasArg("ssid") && server.hasArg("password")) {
    arg1 = server.arg("ssid"), arg2 = server.arg("password");
    if (arg1 == "" || arg2 == "")
      validArgs = false;
  }
  else {
    validArgs = false;
  }

  String jsonResponse = "{\"status\":\"error\"}";
  bool connectNewWifi = false;
  if (validArgs) {
    if (setWifiCredentials(arg1, arg2));{
      jsonResponse = "{\"status\":\"success\"}"; 
      connectNewWifi = true;       
    }    
  }
  
  sendResponse("application/json", jsonResponse);
  if(server.hasArg("connect") && server.arg(2) == "true"){
    connectWithNewCredentials();
  }
}

void postIPToDweet(IPAddress ip) {
  //Client instance to dweet.io
  WiFiClient client;
  char dweet[] = "www.dweet.io";

  String ipStr = ip.toString();
  String post = "POST /dweet/for/";
  post.concat(dweetThingName);
  post.concat("?IP=");
  post.concat(ipStr);
  post.concat("&Hostname=" + WiFi.hostname());

  if (client.connect(dweet, 80)) {
    client.println(post);
    client.println("HOST: www.dweet.io");
    client.println("Connection: close");
    client.println();
  }
}

String formatJson(int greenStatus, int redStatus) {  
  String greenRes = (greenStatus == 1) ? "on" : "off";
  String redRes = (redStatus == 1) ? "on" : "off";

  DynamicJsonBuffer jsonBuffer;
  JsonObject& root = jsonBuffer.createObject();  
  JsonObject& statusObj = root.createNestedObject("status");
  
  statusObj["green"] = greenRes;
  statusObj["red"] = redRes;    
  statusObj["ssid"] = WiFi.SSID();
  statusObj["ip"] = WiFi.localIP().toString();
    
  String json;
  root.printTo(json);
  
  return json;  
}

void handleLightStatus() {
  if (!server.hasArg("green") && !server.hasArg("red")) {
    sendResponse("application/json", "{\"error\":\"invalid_arg\"}");
    return;
  }

  String key = server.argName(0);
  String value = server.arg(0);
  int thisPin = 0;
  int thisState = HIGH;
  int otherPin = 2;
  int inverseState = LOW;

  if (key == "green") {
    //thisPin = 0;
    //otherPin = 2;
  }
  else if (key == "red") {
    thisPin = 2;
    otherPin = 0;
  }
  else {
    sendResponse("application/json", "{\"error\":\"invalid_key\"}");
    return;
  }

  if (value == "on") {
    //thisState = HIGH;
    //inverseState = LOW;
  }
  else if (value == "off") {
    thisState = LOW;
    inverseState = HIGH;
  }
  else {
    sendResponse("application/json", "{\"error\":\"invalid_value\"}");
    return;
  }

  digitalWrite(thisPin, thisState);
  digitalWrite(otherPin, inverseState);
  //TODO: blink led before turn on. Use millis().
  int greenStatus = digitalRead(GREEN_PIN);
  int redStatus = digitalRead(RED_PIN);  

  sendResponse("application/json", formatJson(greenStatus, redStatus));
}

void handleTwoLights() {
  if (!server.hasArg("power")) {
    sendResponse("application/json", "{\"error\":\"invalid_arg\"}");
    return;
  }

  int state = LOW;
  if (server.arg(0) == "on")
    state = HIGH;

  digitalWrite(GREEN_PIN, state);
  digitalWrite(RED_PIN, state);

  sendResponse("application/json", formatJson(state, state));
}

void handleStatus() {
  int greenStatus = digitalRead(GREEN_PIN);
  int redStatus = digitalRead(RED_PIN);
  sendResponse("application/json", formatJson(greenStatus, redStatus));
}

//Restart the device
void handleReset() {  
 digitalWrite(GREEN_PIN, HIGH);
 digitalWrite(RED_PIN, HIGH);
 ESP.restart(); 
}

//Enter the upload mode
void handleBoot() {  
 digitalWrite(GREEN_PIN, HIGH);
 digitalWrite(RED_PIN, LOW);
 ESP.restart(); 
}

void handleNotFound() {
  server.send(404, "text/plain", "Error 404: Page Not Found");
}

void sendResponse(String type, String message) {
  server.send(200, type, message);
}

void startServer() {
  server.on("/reset", handleReset);
  server.on("/boot", handleBoot);
  
  server.on("/status", handleStatus);  
  server.on("/set", handleLightStatus);
  server.on("/switch", handleTwoLights);
  server.on("/", handleIndex);
  server.on("/wifi", handleWifiPage);
  server.onNotFound(handleNotFound);

  server.begin();
  Serial.println("Server started");
}
