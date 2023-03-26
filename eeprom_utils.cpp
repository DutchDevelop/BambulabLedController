#include <EEPROM.h>
#include "eeprom_utils.h"
#include "variables.h"

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

void readEEPROM(String &printerip, String &printercode, String &printerid) {
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

  printerip = ipeeprom;
  printercode = codeeprom;
  printerid = Ideeprom;
}

void writeEEPROM(String printerip, String printercode, String printerid) {
    printerip.replace(".","Q");
  
    String parsediparg = fillWithUnderscores(printerip,Max_ipLength);
    String parsedcodearg = fillWithUnderscores(printercode,Max_accessCode);
    String parsedID = fillWithUnderscores(printerid,Max_DeviceId);

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
}

void clearEEPROM(){
    Serial.println("Clearing EEPROM");
    for (int i = 0; i < 512; i++) {
        EEPROM.write(i, 0);
    }
    EEPROM.commit();
    Serial.println("EEPROM Cleared");
}