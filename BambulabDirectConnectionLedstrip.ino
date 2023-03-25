#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <ESP8266WebServer.h>
#include <EEPROM.h>
#include <WiFiManager.h>
#include <string>
#include <PubSubClient.h>
#include <ArduinoJson.h> 

#define LED_PIN_R 5  // Red pin
#define LED_PIN_G 14 // Green pin
#define LED_PIN_B 4  // Blue pin
#define LED_PIN_W 0  // White pin
#define LED_PIN_WW 2 // Warm white pin

//#define MQTT_MAX_PACKET_SIZE 256
const char* wifiname = "Bambulab Led controller";
const char* setuppage = "<form method='POST' action='/setupmqtt'><label>IP: </label><input type='text' name='ip'><br><label>Access Code: </label><input type='text' name='code'><br><label>Serial ID: </label><input type='text' name='id'><br><input type='submit' value='Save'></form>";
const char* finishedpage = "<h1>Successfully saved paramiters</h1>";

int Max_ipLength = 15;
int Max_accessCode = 8;
int Max_DeviceId = 15;

int Ip_Adress = 0;
int Accesscode_Adress = 15;
int DeviceId_Adress = 23;

String Printerip;
String Printercode;
String PrinterID;

int CurrentStage = -5;
bool ledstate = false;

ESP8266WebServer server(80);
IPAddress apIP(192, 168, 1, 1);

WiFiClientSecure WiFiClient;
PubSubClient mqttClient(WiFiClient);

String fillWithUnderscores(String text, int length) {
    while (text.length() < length) {
        text += "_";
    }
    return text;
}

String removeUnderscores(String text) {
    text.replace("_", "");
    return text;
}

void handleLed(){

}

void handleSetupRoot() {
  server.send(200, "text/html", setuppage);
}

void SetupWebpage(){
  Serial.println("Starting Web server");
  server.on("/", handleSetupRoot);
  server.on("/setupmqtt", savemqttdata);
  server.begin();
  Serial.println("Web server started");
}

void savemqttdata() {
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

  iparg.replace(".","Q");
  
  String parsediparg = fillWithUnderscores(iparg,Max_ipLength);
  String parsedcodearg = fillWithUnderscores(codearg,Max_accessCode);
  String parsedID = fillWithUnderscores(idarg,Max_DeviceId);
  
  Serial.println(parsediparg);
  Serial.println(parsedcodearg);
  Serial.println(parsedID);

  Serial.println("Writing to eeprom");
  
  for (int i = 0; i < parsediparg.length(); i++) {
    EEPROM.write(Ip_Adress + i , parsediparg[i]);
  }

  for (int i = 0; i < parsedcodearg.length(); i++) {
    EEPROM.write(Accesscode_Adress + i, parsedcodearg[i]);
  }

  for (int i = 0; i < parsedID.length(); i++) {
    EEPROM.write(DeviceId_Adress + i, parsedID[i]);
  }

  EEPROM.commit();

  Serial.println("Finished Writing to eeprom");

  readeeprom();
}

void readeeprom(){
  Serial.println("Reading from eeprom");
  String Parsedipeeprom = "";
  for (int i = 0; i < Max_ipLength; i++) {
    char c = EEPROM.read(Ip_Adress+i);
    Parsedipeeprom += c;
  }
  String ipeeprom = removeUnderscores(Parsedipeeprom);
  ipeeprom.replace("Q",".");

  String Parsedcodeeprom = "";
  for (int i = 0; i < Max_accessCode; i++) {
    char c = EEPROM.read(Accesscode_Adress + i);
    Parsedcodeeprom += c;
  }
  String codeeprom = removeUnderscores(Parsedcodeeprom);

  String ParsedIdeeprom = "";
  for (int i = 0; i < Max_DeviceId; i++) {
    char c = EEPROM.read(DeviceId_Adress + i);
    ParsedIdeeprom += c;
  }
  
  String Ideeprom = removeUnderscores(ParsedIdeeprom);

  Serial.println(ipeeprom);
  Serial.println(codeeprom);
  Serial.println(Ideeprom);

  Printerip = ipeeprom;
  Printercode = codeeprom;
  PrinterID = Ideeprom;
}

void PrinterCallback(char* topic, byte* payload, unsigned int length){
  Serial.print("Message arrived in topic: ");
  Serial.println(topic);
  Serial.print("Message:");

  // Parse the JSON object
  StaticJsonDocument<10000> doc;
  DeserializationError error = deserializeJson(doc, payload, length);

  // Check for parsing errors
  if (error) {
    Serial.print("deserializeJson() failed: ");
    Serial.println(error.c_str());
    return;
  }

   if (!doc.containsKey("print")) {
    return;
  }

  // Extract the "stg_cur" value
  int stg_cur = doc["print"]["stg_cur"];

  // Print the value
  Serial.print("stg_cur: ");
  Serial.println(stg_cur);

  if (!doc["print"].containsKey("lights_report")) {
    return;
  }

  bool cur_led = doc["print"]["lights_report"][0]["mode"] == "on";

  // Print the value
  Serial.print("cur_led: ");
  Serial.println(cur_led);

  Serial.println(" - - - - - - - - - - - -");
}

void setup() {
  Serial.begin(115200);
  EEPROM.begin(512);

  pinMode(D8, INPUT_PULLUP);
  if (digitalRead(D8) == HIGH) {
    Serial.println("Clearing EEPROM");
    for (int i = 0; i < 512; i++) {
      EEPROM.write(i, 0);
    }
    EEPROM.commit();
    Serial.println("EEPROM Cleared");
  }

  WiFiClient.setInsecure();
  mqttClient.setBufferSize(10000);
  
  WiFiManager wifiManager;
  wifiManager.autoConnect("MyESP8266");

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

  readeeprom();
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

void loop() {
  server.handleClient();

  if (Printercode.length() > 0 && PrinterID.length() > 0){
    if (!mqttClient.connected()) {
      Serial.println("Connecting to mqtt");
      if (mqttClient.connect("ESP8266-MQTT-c7fad005", "bblp", Printercode.c_str())){
        Serial.println("Connected to MQTT");
        char mqttTopic[50];
        strcpy(mqttTopic, "device/");
        strcat(mqttTopic, PrinterID.c_str());
        strcat(mqttTopic, "/report");
        Serial.println(mqttTopic);
        mqttClient.subscribe(mqttTopic);     
      } else {
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