#include <WiFi.h>             // WiFi control for ESP32
#include <ThingsBoard.h>      // ThingsBoard SDK
#include "esp_wpa2.h"   //wpa2 library for connections to Enterprise networks

#define EAP_IDENTITY ""                
#define EAP_PASSWORD "" // Secure
const char* essid = "eduroam";

// Helper macro to calculate array size
#define COUNT_OF(x) ((sizeof(x)/sizeof(0[x])) / ((size_t)(!(sizeof(x) % sizeof(0[x])))))

// See https://thingsboard.io/docs/getting-started-guides/helloworld/
//


#define TOKEN               "0exNgzuDnwPw7iaUumqi"
#define THINGSBOARD_SERVER  "demo.thingsboard.io"

WiFiClient espClient;           // Initialize ThingsBoard client
ThingsBoard tb(espClient);      // Initialize ThingsBoard instance
int status = WL_IDLE_STATUS;    // the Wifi radio's status

void setup();
void loop();
void InitWifi();

float targetTemperature = 25;
float targetPH = 5;
int targetRPM = 1000;

int quant = 1;                  // Main application loop delay
int updateDelay = 10000;        // Initial update delay.
int lastUpdate  = 0;            // Time of last update.
bool subscribed = false;        // Set to true if application is subscribed for the RPC messages.

RPC_Response onGetTargetTemperature(const RPC_Data &data){
  Serial.print("getTargetTemperature: ");
  Serial.println(targetTemperature);
  
  return RPC_Response(NULL, targetTemperature);
}

RPC_Response onSetTargetTemperature(const RPC_Data &data){
  Serial.print("setTargetTemperature: ");
  
  // Process data
  targetTemperature = data;
  Serial.println(targetTemperature);

  return RPC_Response(NULL, targetTemperature);
}

RPC_Response onGetTargetPH(const RPC_Data &data){
  Serial.print("getTargetPH: ");
  Serial.println(targetPH);
  
  return RPC_Response(NULL, targetPH);
}

RPC_Response onSetTargetPH(const RPC_Data &data){
  Serial.print("setTargetPH: ");
  
  // Process data
  targetPH = data;
  Serial.println(targetPH);

  return RPC_Response(NULL, targetPH);
}

RPC_Response onGetTargetRPM(const RPC_Data &data){
  Serial.print("getTargetRPM: ");
  Serial.println(targetRPM);
  
  return RPC_Response(NULL, targetRPM);
}

RPC_Response onSetTargetRPM(const RPC_Data &data){
  Serial.print("setTargetRPM: ");
  
  // Process data
  targetRPM = data;
  Serial.println(targetRPM);

  return RPC_Response(NULL, targetRPM);
}

// RPC handlers
RPC_Callback callbacks[] = {
  {"getTargetTemperature", onGetTargetTemperature },
  {"setTargetTemperature", onSetTargetTemperature },
  {"getTargetPH", onGetTargetPH },
  {"setTargetPH", onSetTargetPH },
  {"getTargetRPM", onGetTargetRPM },
  {"setTargetRPM", onSetTargetRPM},
};

void InitWifi(){
  bool eduroamFound = false;
  delay(10);
  // Scan available WiFi networks until eduroam is seen
  
  // Set WiFi to station mode and disconnect from an AP if it was previously connected
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();

  // Repeatedly scan until we see eduroam
  //
  while (!eduroamFound) {
    Serial.println("scan start");
    int n = WiFi.scanNetworks(); // WiFi.scanNetworks returns the number of networks found
    Serial.println("scan done");
    
    if (n == 0) {
        Serial.println("no networks found");
    } else {
        Serial.print(n);
        Serial.println(" networks found");
        
        for (int i = 0; i < n; ++i) {
            String ssid = WiFi.SSID(i);
            int    rssi = WiFi.RSSI(i);
          
            // Print SSID and RSSI for each network found
            Serial.print(i + 1);
            Serial.print(": ");
            Serial.print(ssid);
            Serial.print(" (");
            Serial.print(rssi);
            Serial.print(")");
            Serial.print((WiFi.encryptionType(i) == WIFI_AUTH_OPEN)?" ":"*");
            delay(10);
            
            ssid.trim();
            if (ssid == essid) {
              Serial.print(" <==== eduroam found");
              eduroamFound = true;
            }
            Serial.println("");
        }
    }
    Serial.println("");

    // Wait a bit before scanning again
    if (!eduroamFound)
      delay(5000);
  }
  
  // If we come here, we've successfully found eduroam. Try to connect to it.
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(essid);
  
  // Set WiFi to station mode and disconnect from an AP if it was previously connected
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();

  // This is where the wpa2 magic happens to allow us to connect to eduroam
  esp_wifi_sta_wpa2_ent_set_username((uint8_t *)EAP_IDENTITY, strlen(EAP_IDENTITY));
  esp_wifi_sta_wpa2_ent_set_password((uint8_t *)EAP_PASSWORD, strlen(EAP_PASSWORD));
  esp_wifi_sta_wpa2_ent_enable();
  
  WiFi.begin(essid);       //connect to eduroam
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("WiFi is connected to ");
  Serial.println(essid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP()); //print LAN IP
}

void setup() {
  Serial.begin(115200);
  InitWifi();
}

void loop() {
  long now = millis();

  delay(quant);

  // Reconnect to ThingsBoard, if needed
  if (!tb.connected()) {
    subscribed = false;

    // Connect to the ThingsBoard
    Serial.print("Connecting to: ");
    Serial.print(THINGSBOARD_SERVER);
    Serial.print(" with token ");
    Serial.println(TOKEN);
    if (!tb.connect(THINGSBOARD_SERVER, TOKEN)) {
      Serial.println("Failed to connect");
      return;
    }
  }  

  // Subscribe for RPC, if needed
  if (!subscribed) {
    Serial.println("Subscribing for RPC...");

    // Perform a subscription. All consequent data processing will happen in
    // callbacks as denoted by callbacks[] array.
    if (!tb.RPC_Subscribe(callbacks, COUNT_OF(callbacks))) {
      Serial.println("Failed to subscribe for RPC");
      return;
    }

    Serial.println("Subscribe done");
    subscribed = true;
  }

  if (now > lastUpdate + updateDelay) {
    // Uploads new telemetry to ThingsBoard using MQTT. 
    // See https://thingsboard.io/docs/reference/mqtt-api/#telemetry-upload-api 
    // for more details   
    tb.sendTelemetryFloat("temperature", (float)getCurrentTemperature() / 10);
    tb.sendTelemetryFloat("pH", (float)getCurrentPH() / 10);
    tb.sendTelemetryInt("stirring", getCurrentRPM());
    
    lastUpdate += updateDelay;
  }

  // Process messages
  tb.loop();   

}


int getCurrentTemperature(){
  int temperature = ((float)rand()/(float)(RAND_MAX)) * 60 * 10; // Multiple by 10 for fixed point noation
  return temperature;
}

int getCurrentPH(){
  int ph = ((float)rand()/(float)(RAND_MAX)) * 14 * 10; // Multiply by 10 for fixed point notation
  return ph;
}

int getCurrentRPM(){
  int rpm = random(2000);
  return rpm;
}
