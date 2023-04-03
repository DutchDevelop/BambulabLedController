#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <ESP8266WebServer.h>
#include <WiFiUdp.h>
#include <WiFiManager.h>
#include <ESP8266mDNS.h>
#include <string>
#include <PubSubClient.h>
#include <ArduinoJson.h> 
#include <EEPROM.h>
#include "eeprom_utils.h"
#include "led_utils.h"
#include "variables.h"
#include "html.h"

const char* wifiname = "Bambulab Led controller";
const char* setuppage = html_setuppage;
const char* finishedpage = html_finishpage;

String Printerip;
String Printercode;
String PrinterID;

int CurrentStage = -1;
bool hasHMSerror = false;
bool ledstate = false;
unsigned long finishstartms;

ESP8266WebServer server(80);
IPAddress apIP(192, 168, 1, 1);

WiFiClientSecure WiFiClient;
WiFiManager wifiManager;
PubSubClient mqttClient(WiFiClient);

String generateRandomString(int length) { //Function to generate random string for MQTT
  char charset[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
  int charsetLength = strlen(charset);

  String randomString = "";
  for (int i = 0; i < length; i++) {
    int randomIndex = random(0, charsetLength);
    randomString += charset[randomIndex];
  }
  
  return randomString;
}

void handleLed(){ //Function to handle ledstatus eg if the X1C has an error then make the ledstrip red, or when its scanning turn off the light until its starts printing
  if (ledstate == 1){
    if (CurrentStage == 6 || CurrentStage == 17 || CurrentStage == 20 || CurrentStage == 21 || hasHMSerror){
      setLedColor(255,0,0,0,0);
      return;
    };
    if (finishstartms > 0 && millis() - finishstartms <= 300000){
      setLedColor(0,255,0,0,0);
      return;
    }else if(millis() - finishstartms > 300000){
      finishstartms = 0;
    }
    if (CurrentStage == 0 || CurrentStage == -1 || CurrentStage == 2){
      setLedColor(0,0,0,255,255);
      return;
    };
    if (CurrentStage == 14 || CurrentStage == 9){
      setLedColor(0,0,0,0,0);
      return;
    };
  }else{
    setLedColor(0,0,0,0,0);
  };
}

void handleSetupRoot() { //Function to handle the setuppage
  server.send(200, "text/html", setuppage);
}

void SetupWebpage(){ //Function to start webpage system
  Serial.println("Starting Web server");
  server.on("/", handleSetupRoot);
  server.on("/setupmqtt", savemqttdata);
  server.begin();
  Serial.println("Web server started");
}

void savemqttdata() { //Function to handle given information from the setuppage and stores them into eeprom which then reads them from eeprom
  String iparg = server.arg("ip");
  String codearg = server.arg("code");
  String idarg = server.arg("id");  

  if (iparg.length() == 0){
    return handleSetupRoot();
  };

  if (codearg.length() == 0){
    return handleSetupRoot();
  };

  if (idarg.length() == 0){
    return handleSetupRoot();
  };

  server.send(200, "text/html", finishedpage);

  Serial.println("Printer IP:");
  Serial.println(iparg);
  Serial.println("Printer Code:");
  Serial.println(codearg);
  Serial.println("Printer Id:");
  Serial.println(idarg);

  writeEEPROM(iparg,codearg,idarg);

  readEEPROM(Printerip,Printercode,PrinterID);

}

void PrinterCallback(char* topic, byte* payload, unsigned int length){ //Function to handle the MQTT Data from the mqtt broker
  if (length < 250) { //Ignore the MC_Print message
    return;
  }
  Serial.print("Message arrived in topic: ");
  Serial.println(topic);
  Serial.println(length);
  Serial.print("Message:");

  StaticJsonDocument<10000> doc;
  DeserializationError error = deserializeJson(doc, payload, length);

  if (error) {
    Serial.print("deserializeJson() failed: ");
    Serial.println(error.c_str());
    return;
  }

   if (!doc.containsKey("print")) {
    return;
  }

  CurrentStage = doc["print"]["stg_cur"];

  Serial.print("stg_cur: ");
  Serial.println(CurrentStage);

  if (!doc["print"].containsKey("lights_report")) {
    return;
  }

  ledstate = doc["print"]["lights_report"][0]["mode"] == "on";

  Serial.print("cur_led: ");
  Serial.println(ledstate);

  //if (doc["print"]["gcode_state"] == "FINISH" && finishstartms <= 0){
    //finishstartms = millis();
  //}else if (doc["print"]["gcode_state"] != "FINISH" && finishstartms > 0){
    //finishstartms = 0;
 // }
  
  hasHMSerror = false;

  for (const auto& hms : doc["print"]["hms"].as<JsonArray>()) {
      if (hms["code"] == 131073) {
        hasHMSerror = true;
      };
  }

  Serial.print("HMS error: ");
  Serial.println(hasHMSerror);

  Serial.println(" - - - - - - - - - - - -");

  handleLed();
}

void setup() { // Setup function
  Serial.begin(115200);
  EEPROM.begin(512);

  pinMode(D8, INPUT_PULLUP);

  if (digitalRead(D8) == HIGH) {
    clearEEPROM();
  }

  setPins(0,0,0,0,0);

  WiFiClient.setInsecure();
  mqttClient.setBufferSize(10000);
  
  wifiManager.autoConnect(wifiname);

  MDNS.begin("bambuledcontroller");
  WiFi.hostname("bambuledcontroller");

  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("Failed to connect to WiFi, creating access point...");
    WiFiManager wifiManager;
    wifiManager.setAPCallback([](WiFiManager* mgr) {
      Serial.println("Access point created, connect to:");
      Serial.print(mgr->getConfigPortalSSID());
    });
    wifiManager.setConfigPortalTimeout(300);
    wifiManager.startConfigPortal(wifiname);
  }

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to Wi-Fi...");
  }

  Serial.print("Connected to WiFi, IP address: ");
  Serial.println(WiFi.localIP());

  readEEPROM(Printerip,Printercode,PrinterID);
  SetupWebpage();

  while (Printerip.length() == 0){
    delay(1000);
    Serial.println("Waiting for printer IP");
  }

  Serial.println("Has Ip starting mqtt server");

  mqttClient.setServer(Printerip.c_str(), 8883);
  Serial.println(Printerip.c_str());
  mqttClient.setCallback(PrinterCallback);
}

void loop() { //Loop function
  server.handleClient();
  if (Printercode.length() > 0 && PrinterID.length() > 0){
    if (!mqttClient.connected()) {
      char DeviceName[50];
      strcpy(DeviceName, "ESP8266-MQTT-");
      strcat(DeviceName, generateRandomString(10).c_str());
      Serial.print("Connecting with device name:");
      Serial.println(DeviceName);
      Serial.println("Connecting to mqtt");
      if (mqttClient.connect(DeviceName, "bblp", Printercode.c_str())){
        Serial.println("Connected to MQTT");
        setLedColor(0,0,0,0,0); //Turn off led printer might be offline
        char mqttTopic[50];
        strcpy(mqttTopic, "device/");
        strcat(mqttTopic, PrinterID.c_str());
        strcat(mqttTopic, "/report");
        Serial.println("Topic: ");
        Serial.println(mqttTopic);
        mqttClient.subscribe(mqttTopic);     
      } else {
        setLedColor(0,0,0,0,0); //Turn off led printer is offline and or the given information is wrong
        Serial.print("failed, rc=");
        Serial.print(mqttClient.state());
        Serial.println(" try again in 5 seconds");
        delay(5000);
      }
    }
  } else {
    Serial.println("No printercode and or printer id present.");
  }
  mqttClient.loop();
}