#ifndef EEPROM_UTILS_H
#define EEPROM_UTILS_H

#include <Arduino.h>

void readEEPROM(String &printerip,String &printercode, String &printerid, String &EspPassword);

void writeEEPROM(String printerip, String printercode, String printerid, String EspPassword);

void clearEEPROM();

#endif // EEPROM_UTILS_H