#ifndef EEPROM_UTILS_H
#define EEPROM_UTILS_H

#include <Arduino.h>

void readFromEEPROM(char* Printerip, char* Printercode, char* PrinterID, char* EspPassword);

void writeToEEPROM(char* Printerip, char* Printercode, char* PrinterID, char* EspPassword);

void clearEEPROM();

#endif // EEPROM_UTILS_H