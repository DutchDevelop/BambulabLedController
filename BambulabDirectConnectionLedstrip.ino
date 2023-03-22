#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <ESP8266WebServer.h>
#include <EEPROM.h>
#include <WiFiManager.h>
#include <string>
#include <PubSubClient.h>
#include <ArduinoJson.h> 

#define MQTT_VERSION MQTT_VERSION_3_1_1
#define MQTT_MAX_PACKET_SIZE 500 

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
  char payload_buffer[length + 1];
  memcpy(payload_buffer, payload, length);
  payload_buffer[length] = '\0';

  // Parse the JSON data
  StaticJsonDocument<500> doc;
  DeserializationError error = deserializeJson(doc, payload_buffer);

  if (error) {
    Serial.print("Failed to parse JSON payload: ");
    Serial.println(error.c_str());
    return;
  }

  Serial.println("Parsed JSON object:");
  serializeJsonPretty(doc, Serial);
  Serial.println();

}

void setup() {
  Serial.begin(115200);
  EEPROM.begin(512);

  WiFiClient.setInsecure();

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