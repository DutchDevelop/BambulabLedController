#ifndef VARIABLES_H
#define VARIABLES_H

#include <Arduino.h>

extern String Printerip;
extern String Printercode;
extern String PrinterID;

extern int CurrentStage;
extern bool ledstate;

const int Max_ipLength = 15;
const int Max_accessCode = 8;
const int Max_DeviceId = 15;
const int Max_Bool = 1;

const int Ip_Adress = 0;
const int Accesscode_Adress = 15;
const int DeviceId_Adress = 23;
const int Allow_Api_Requests = 38;


#endif