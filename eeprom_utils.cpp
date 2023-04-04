#include <EEPROM.h>
#include "eeprom_utils.h"
#include "variables.h"

void fillWithUnderscores(char* text, int length) { //Fills whitespace with underscores for eeprom to overwrite
    int textLength = strlen(text);
    for(int i=textLength; i<length; i++) {
        text[i] = '_';
    }
    text[length] = '\0'; // add null terminator
}

void removeUnderscores(char* text) { //Removes underscores from message
  for(int i = 0; i < strlen(text); i++){
    if(text[i] == '_'){
      for(int j = i; j < strlen(text); j++){
        text[j] = text[j+1];
      }
      i--;
    }
  }
}

void readFromEEPROM(char* Printerip, char* Printercode, char* PrinterID, char* EspPassword) { //Function to write the PrinterID, Printerip and AccessCode to the eeprom

  Serial.println("Reading from eeprom");
  char Parsedipeeprom[Max_ipLength+1] = "";
  for (int i = 0; i < Max_ipLength; i++) {
    Parsedipeeprom[i] = EEPROM.read(Ip_Adress+i);
  }
  Parsedipeeprom[Max_ipLength] = '\0';
  removeUnderscores(Parsedipeeprom);
  for(int i = 0; i < strlen(Parsedipeeprom); i++){
    if(Parsedipeeprom[i] == 'Q'){
      Parsedipeeprom[i] = '.';
    }
  }
  
  char Parsedcodeeprom[Max_accessCode+1] = "";
  for (int i = 0; i < Max_accessCode; i++) {
    Parsedcodeeprom[i] = EEPROM.read(Accesscode_Adress + i);
  }
  Parsedcodeeprom[Max_accessCode] = '\0';
  removeUnderscores(Parsedcodeeprom);

  char ParsedIdeeprom[Max_DeviceId+1] = "";
  for (int i = 0; i < Max_DeviceId; i++) {
    ParsedIdeeprom[i] = EEPROM.read(DeviceId_Adress + i);
  }
  ParsedIdeeprom[Max_DeviceId] = '\0';
  removeUnderscores(ParsedIdeeprom);

  char ParsedEspPassword[Max_EspPassword+1] = "";
  for (int i = 0; i < Max_EspPassword; i++) {
    ParsedEspPassword[i] = EEPROM.read(EspPassword_Adress + i);
  }
  ParsedEspPassword[Max_EspPassword] = '\0';

  Serial.println(Parsedipeeprom);
  Serial.println(Parsedcodeeprom);
  Serial.println(ParsedIdeeprom);
  Serial.println(ParsedEspPassword);

  strcpy(Printerip, Parsedipeeprom);
  strcpy(Printercode, Parsedcodeeprom);
  strcpy(PrinterID, ParsedIdeeprom);
  strcpy(EspPassword, ParsedEspPassword);
}

void writeToEEPROM(char* Printerip, char* Printercode, char* PrinterID, char* EspPassword) { //Function to read the PrinterID, Printerip and AccessCode from the eeprom
    int ipLength = strlen(Printerip);
    int codeLength = strlen(Printercode);
    int idLength = strlen(PrinterID);
  
    char parsediparg[Max_ipLength+1];
    char parsedcodearg[Max_accessCode+1];
    char parsedID[Max_DeviceId+1];
  
    strcpy(parsediparg, Printerip);
    strcpy(parsedcodearg, Printercode);
    strcpy(parsedID, PrinterID);

    fillWithUnderscores(parsediparg, Max_ipLength);
    fillWithUnderscores(parsedcodearg, Max_accessCode);
    fillWithUnderscores(parsedID, Max_DeviceId);
  
    Serial.println(parsediparg);
    Serial.println(parsedcodearg);
    Serial.println(parsedID);

    Serial.println("Writing to eeprom");

    for (int i = 0; i < ipLength; i++) {
        EEPROM.write(Ip_Adress + i , parsediparg[i]);
    }

    for (int i = 0; i < codeLength; i++) {
        EEPROM.write(Accesscode_Adress + i, parsedcodearg[i]);
    }

    for (int i = 0; i < idLength; i++) {
        EEPROM.write(DeviceId_Adress + i, parsedID[i]);
    }

    for (int i = 0; i < strlen(EspPassword); i++) {
        EEPROM.write(EspPassword_Adress + i, EspPassword[i]);
    }

    EEPROM.commit();

    Serial.println("Finished Writing to eeprom");
}

void clearEEPROM(){ //Incase eeprom gets messed up set pin 6 to high to clear data
    Serial.println("Clearing EEPROM");
    for (int i = 0; i < 512; i++) {
        EEPROM.write(i, 0);
    }
    EEPROM.commit();
    Serial.println("EEPROM Cleared");
}