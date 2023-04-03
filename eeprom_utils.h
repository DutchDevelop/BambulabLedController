#ifndef EEPROM_UTILS_H
#define EEPROM_UTILS_H

#include <Arduino.h>

void readEEPROM(String &printerip,String &printercode, String &printerid, bool &Allow_Api);

void writeEEPROM(String printerip, String printercode, String printerid, bool Allow_Api);

void clearEEPROM();

#endif // EEPROM_UTILS_H