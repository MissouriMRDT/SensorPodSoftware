#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <IPAddress.h>
#include "RoveBoard.h"
#include "RoveComm.h"


#define TEMPERATURE_PIN 1
#define MOISTURE_PIN    2
#define ROVE_SSID       RoveSoHard
#define ROVE_PASSWORD   RoveForever

#define DELAY_MILLIS   100

#define TEMPERATURE_DATA_ID 100
#define MOISTURE_DATA_ID    101

//IP Address
#define FIRST_OCTET    192
#define SECOND_OCTET   168
#define THIRD_OCTET    1
#define FOURTH_OCTET   100


void setup() {
  pinMode(TEMPERATURE_PIN, OUTPUT);
  pinMode(MOISTURE_PIN, OUTPUT);
  digitalWrite(TEMPERATURE_PIN, 0);
  digitalWrite(MOISTURE_PIN, 0);
  pinMode(A0, INPUT);

  roveComm_BeginWifi(ROVE_SSID, ROVE_PASSWORD, FIRST_OCTET, SECOND_OCTET, THIRD_OCTET, FOURTH_OCTET);
  
}

void loop() {
  //Read Temperature Sensor
  digitalWrite(TEMPERATURE_PIN, 1);
  delay(DELAY_MILLIS);
  roveComm_GetMsg();
  roveComm_SendMsg(TEMPERATURE_DATA_ID, sizeof(int), analogRead(A0));
  digitalWrite(TEMPERATURE_PIN, 0);

  //Read Moisture Sensor
  digitalWrite(MOISTURE_PIN, 1);
  delay(DELAY_MILLIS);
  roveComm_GetMsg();
  roveComm_SendMsg(MOISTURE_DATA_ID, sizeof(int), analogRead(A0)); 
  digitalWrite(MOISTURE_PIN, 0);

}
