#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <IPAddress.h>
#include "RoveBoard.h"
#include "RoveComm.h"


#define TEMPERATURE_PIN 1
#define MOISTURE_PIN    2

#define DELAY_MILLIS   100

#define TEMPERATURE_DATA_ID 100
#define MOISTURE_DATA_ID    101

uint16_t data_id;
size_t   data_size;
uint8_t  data_value;
int      reading;

//IP Address
unsigned char FIRST_OCTET  =  192;
unsigned char SECOND_OCTET =  168;
unsigned char THIRD_OCTET  =  1;
unsigned char FOURTH_OCTET =  100;

char ROVE_SSID[] =      "RoveSoHard";
char ROVE_PASSWORD[] =  "Illuminati";
  
void setup() {
  pinMode(TEMPERATURE_PIN, OUTPUT);
  pinMode(MOISTURE_PIN, OUTPUT);
  digitalWrite(TEMPERATURE_PIN, 0);
  digitalWrite(MOISTURE_PIN, 0);
  pinMode(A0, INPUT);

  roveComm_BeginWiFi(ROVE_SSID, ROVE_PASSWORD, FIRST_OCTET, SECOND_OCTET, THIRD_OCTET, FOURTH_OCTET);
  
}

void loop() {
  //Read Temperature Sensor
  digitalWrite(TEMPERATURE_PIN, 1);
  delay(DELAY_MILLIS);
  roveComm_GetMsg(&data_id, &data_size, &data_value);
  reading = analogRead(A0);
  roveComm_SendMsg(TEMPERATURE_DATA_ID, sizeof(int), &reading);
  digitalWrite(TEMPERATURE_PIN, 0);

  //Read Moisture Sensor
  digitalWrite(MOISTURE_PIN, 1);
  delay(DELAY_MILLIS);
  roveComm_GetMsg(&data_id, &data_size, &data_value);
  reading = analogRead(A0);
  roveComm_SendMsg(MOISTURE_DATA_ID, sizeof(int), &reading); 
  digitalWrite(MOISTURE_PIN, 0);

}
