#include <Arduino.h>
#include "led_utils.h"

#define LED_PIN_R 5  // Red pin
#define LED_PIN_G 14 // Green pin
#define LED_PIN_B 4  // Blue pin
#define LED_PIN_W 0  // White pin
#define LED_PIN_WW 2 // Warm white pin

int startR = 0;
int startG = 0;
int startB = 0;
int startC = 0;
int startW = 0;

void setLedColor(int redValue, int greenValue, int blueValue, int coldValue, int warmValue) {
   transitionLedColor(redValue,greenValue,blueValue,coldValue,warmValue,100);
}

void setPins(int redValue, int greenValue, int blueValue, int coldValue, int warmValue) {
    pinMode(LED_PIN_R, OUTPUT);
    pinMode(LED_PIN_G, OUTPUT);
    pinMode(LED_PIN_B, OUTPUT);
    pinMode(LED_PIN_W, OUTPUT);
    pinMode(LED_PIN_WW, OUTPUT);

    analogWrite(LED_PIN_R, redValue);
    analogWrite(LED_PIN_G, greenValue);
    analogWrite(LED_PIN_B, blueValue);
    analogWrite(LED_PIN_W, coldValue);
    analogWrite(LED_PIN_WW, warmValue);
}

void transitionLedColor(int endR, int endG, int endB, int endC, int endW, int duration) {
    float stepTime = (float)duration / 255.0; // Time between steps in milliseconds
    int rStep = (endR - startR) / 255; // Step size for red component
    int gStep = (endG - startG) / 255; // Step size for green component
    int bStep = (endB - startB) / 255; // Step size for blue component
    int cStep = (endC - startC) / 255; // Step size for cold white component
    int wStep = (endW - startW) / 255; // Step size for warm white component
    
    for (int i = 0; i < 256; i++) {
        int r = startR + i * rStep;
        int g = startG + i * gStep;
        int b = startB + i * bStep;
        int c = startC + i * cStep;
        int w = startW + i * wStep;
        
        setPins(r, g, b, c, w);
        delay(stepTime);
    }
    startR = endR;
    startG = endG;
    startB = endB;
    startC = endC;
    startW = endW;
}