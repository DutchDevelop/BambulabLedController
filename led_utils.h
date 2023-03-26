#ifndef LED_UTILS_H
#define LED_UTILS_H

#include <Arduino.h>

void setLedColor(int redValue, int greenValue, int blueValue, int coldValue, int warmValue);
void updateLeds();

#endif