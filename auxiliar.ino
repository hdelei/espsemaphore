/*
   This sketch keeps all requests and auxiliar functions
*/

//Thing name in dweet.io 
//It's possible create your own thing name for dweet.io
//const String dweetThingName = "ESP" + String(ESP.getChipId()); 
const String dweetThingName = "dojo-grupy";//custom name

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

//List available Networks 
bool listNetworks(String prevSSID) {
  bool ssidMatch = false;
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
    if(WiFi.SSID(thisNet) == prevSSID){
      ssidMatch = true;
    }
  }
  return ssidMatch;
}

//Persist new credentials in JSON file
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
  Serial.println("");

  return true;
}

String getStoredCredentials(String type) {
  //Retrieve stored credentials setted in WiFiManager

  String filename = "/config.json";  

  SPIFFS.begin();
  File file;

  if (SPIFFS.exists(filename))
    file = SPIFFS.open(filename, "r");

  DynamicJsonBuffer jsonBuffer;
  JsonObject& root = jsonBuffer.parseObject(file);
  
  if (!root.success()) {
    Serial.println("Can't parse json file");
    return "";
  }

  type = root[type].as<String>();  

  file.close();
  SPIFFS.end();  
  return type;
}

//void connectWithNewCredentials() {
//  String filename = "/config.json";
//
//  SPIFFS.begin();
//  File file;
//
//  if (SPIFFS.exists(filename))
//    file = SPIFFS.open(filename, "r");
//
//  DynamicJsonBuffer jsonBuffer;
//  JsonObject& root = jsonBuffer.parseObject(file);
//
//  if (!root.success()) {
//    Serial.println("Can't parse json file");
//    return;
//  }
//
//  String ssid = root["ssid"].as<String>();
//  String password = root["password"].as<String>();
//
//  file.close();
//  SPIFFS.end();
//
//  WiFi.mode(WIFI_OFF);
//  Serial.println("");
//  Serial.println("Wifi off. Wait 20 seconds for reconnection...");
//  delay(1000 * 10);
//  Serial.println("Starting Wifi connection with new credentials...");
//
//  WiFi.mode(WIFI_STA);
//  WiFi.begin(ssid.c_str(), password.c_str());
//  Serial.println("");
//  Serial.println("Trying to reconnect...");
//  delay(100);
//}

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

void handleHelpDiv() {
  SPIFFS.begin();
  File htmlDiv;
  String text = "Error reading help";
  if (SPIFFS.exists("/helpdiv.txt")) {
    htmlDiv = SPIFFS.open("/helpdiv.txt", "r");
    text =  htmlDiv.readString();
    htmlDiv.close();
  }
  SPIFFS.end();

  sendResponse("text/html", text);
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
    Serial.print("Post to dweet.io: http://dweet.io/follow/" + dweetThingName);
    Serial.println(" successfully!");
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
  server.on("/help", handleHelpDiv);
  //server.on("/wifi", handleWifiPage);
  server.onNotFound(handleNotFound);

  server.begin();
  Serial.println("Server started");
}
