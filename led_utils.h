#ifndef LED_UTILS_H
#define LED_UTILS_H

#include <Arduino.h>

void setLedColor(int redValue, int greenValue, int blueValue, int coldValue, int warmValue);
void pulseLedColor(int redValue, int greenValue, int blueValue, int coldValue, int warmValue);
void setPins(int redValue, int greenValue, int blueValue, int coldValue, int warmValue);
void transitionLedColor(int endR, int endG, int endB, int endC, int endW, int duration);

#endif