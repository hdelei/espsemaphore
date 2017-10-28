/*
 * This sketch keeps all functions and networks 
 */

//keeps the sizes of wifi repository for easy declaration
const int COLS = 2;
const int ROWS = 3;

//Wifi SSID and PASSWORD repository
const char* WIFI_REPO[COLS][ROWS] = {
  {"SSID1", "SSID2", "SSID3"},
  {"password1", "password2", "password3"}
};


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

void postIPToDweet(IPAddress ip){
  //Client instance to dweet.io
  WiFiClient client;
  char dweet[] = "www.dweet.io"; 

  String post = "POST /dweet/for/grupyespsemaphore?IP=";
  String ipStr = ip.toString();
  post.concat(ipStr);
    
  if (client.connect(dweet, 80)){                  
      client.println(post);
      client.println("HOST: www.dweet.io");
      client.println("Connection: close");
      client.println();      
    }
}

