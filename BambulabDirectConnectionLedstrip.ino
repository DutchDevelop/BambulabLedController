#include <FS.h>
#include <WiFiManager.h>  
#include <PubSubClient.h>
#include <ArduinoJson.h> 
#include <WiFiClientSecure.h>

char printer_ip[16];
char access_code[10];
char serial_id[16];

bool shouldSaveConfig = false;

WiFiClientSecure WiFiClient;
PubSubClient mqttClient(WiFiClient);

void saveConfigCallback () {
  Serial.println("Should save config");
  shouldSaveConfig = true;
}

void setup() {
  Serial.begin(115200);
  Serial.println();
  Serial.println("mounting FS");

  WiFiClient.setInsecure();
  mqttClient.setBufferSize(14000);

  if (SPIFFS.begin()) {
    Serial.println("mounted FS");
    if (SPIFFS.exists("/config.json")) {
      Serial.println("reading config file");
      File configFile = SPIFFS.open("/config.json", "r");
      if (configFile) {
        Serial.println("opened config file");
        size_t size = configFile.size();
        std::unique_ptr<char[]> buf(new char[size]);

        configFile.readBytes(buf.get(), size);
        DynamicJsonDocument jsonDoc(1024);
        DeserializationError error = deserializeJson(jsonDoc, buf.get());
        if (!error) {
          Serial.println("\nparsed json");
          strcpy(printer_ip, jsonDoc["printer_ip"].as<const char*>());
          strcpy(access_code, jsonDoc["access_code"].as<const char*>());
          strcpy(serial_id, jsonDoc["serial_id"].as<const char*>());
        } else {
          Serial.println("failed to load json config");
        }
      }
    }
  } else {
    Serial.println("failed to mount FS");
  }

  WiFiManagerParameter custom_mqtt_ip("printer_ip", "printer ip", printer_ip, 15);
  WiFiManagerParameter custom_mqtt_code("access_code", "local access code", access_code, 8);
  WiFiManagerParameter custom_mqtt_serial("serial_id", "serial id", serial_id, 15);

  WiFiClient.setInsecure();
  WiFiManager wifiManager;

  wifiManager.setSaveConfigCallback(saveConfigCallback);

  wifiManager.addParameter(&custom_mqtt_ip);
  wifiManager.addParameter(&custom_mqtt_code);
  wifiManager.addParameter(&custom_mqtt_serial);


  if (wifiManager.getWiFiIsSaved()) wifiManager.setEnableConfigPortal(false);
  wifiManager.autoConnect("BLLEDController");

  if (WiFi.status() != WL_CONNECTED) {
    Serial.println(F("Failed to connect to WiFi, creating access point..."));
    wifiManager.setAPCallback([](WiFiManager* mgr) {
      Serial.println(F("Access point created, connect to:"));
      Serial.print(mgr->getConfigPortalSSID());
    });
    wifiManager.setConfigPortalTimeout(300);
    wifiManager.startConfigPortal("BLLEDController");
  }

  strcpy(printer_ip, custom_mqtt_ip.getValue());
  strcpy(access_code, custom_mqtt_code.getValue());
  strcpy(serial_id, custom_mqtt_serial.getValue());

  Serial.println(custom_mqtt_ip.getValue());
  Serial.println(custom_mqtt_code.getValue());
  Serial.println(custom_mqtt_serial.getValue());

  Serial.println("------------------------------------");

  Serial.println(printer_ip);
  Serial.println(access_code);
  Serial.println(serial_id);
  
  
  if (shouldSaveConfig) {
    Serial.println("saving config");
    DynamicJsonDocument jsonDoc(1024);
    jsonDoc["printer_ip"] = printer_ip;
    jsonDoc["access_code"] = access_code;
    jsonDoc["serial_id"] = serial_id;

    File configFile = SPIFFS.open("/config.json", "w");
    if (!configFile) {
      Serial.println("failed to open config file for writing");
    }

    serializeJson(jsonDoc, configFile);
    serializeJson(jsonDoc, Serial);
    configFile.close();
  }

  mqttClient.setServer(printer_ip, 8883);
  mqttClient.setCallback(PrinterCallback);
}

void PrinterCallback(char* topic, byte* payload, unsigned int length){
  if (length < 7682) {
    return;
  }
  Serial.print(F("Message arrived in topic: "));
  Serial.println(topic);
  Serial.print(F("Message Length: "));
  Serial.println(length);
}

void mqttreconnect(){
  char mqttTopic[30];
  strcpy(mqttTopic, "device/");
  strcat(mqttTopic, serial_id);
  strcat(mqttTopic, "/report");
  while (!mqttClient.connected()) {
    Serial.print("Attempting MQTT connection...");
    if (mqttClient.connect("ESP8266Client", "bblp", access_code)) {
      Serial.println("connected");
      Serial.println("Subscribing to topic");
      mqttClient.subscribe(mqttTopic);
      Serial.println("Subscribed to topic");
    } else {
      Serial.print("failed, rc=");
      Serial.print(mqttClient.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}

void loop() {
  if (!mqttClient.connected()) {
    mqttreconnect();
  }
  mqttClient.loop();
}