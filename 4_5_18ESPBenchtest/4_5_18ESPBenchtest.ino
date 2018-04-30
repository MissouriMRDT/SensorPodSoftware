#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <IPAddress.h>
#include "RoveBoard.h"
#include "RoveComm.h"


char ROVE_SSID[] =      "MRDT Wifi";
char ROVE_PASSWORD[] =  "Rovin2012";

uint16_t data_id;
size_t   data_size;
uint8_t  data_value;
int      reading;

//IP Address
unsigned char FIRST_OCTET  =  192;
unsigned char SECOND_OCTET =  168;
unsigned char THIRD_OCTET  =  1;
unsigned char FOURTH_OCTET =  132;

const uint16_t GPS_ALTITUDE_DATA_ID = 1120;
const uint16_t GPS_SPEED_DATA_ID = 1104;

uint16_t staticNumber =15;
uint16_t i=1;

void setup() {
Serial.begin(115200);
  Serial.println();
  Serial.println();
pinMode(1, OUTPUT);
pinMode(3, OUTPUT);
pinMode(4, OUTPUT);
pinMode(5, OUTPUT);
delay(100);
digitalWrite(1, 0);
digitalWrite(3, 0);
digitalWrite(4, 0);
digitalWrite(5, 0);


delay(100);
digitalWrite(1, 1);
roveComm_BeginWiFi(ROVE_SSID, ROVE_PASSWORD, 192,168,1,132);
delay(100);

}
void loop(){
if (i==1) Serial.println("roveComm Connected");

 i++;
 roveComm_GetMsg(&data_id, &data_size, &data_value);
 roveComm_SendMsg(GPS_ALTITUDE_DATA_ID, sizeof(uint16_t), &i);
 roveComm_SendMsg(GPS_SPEED_DATA_ID, sizeof(uint16_t), &staticNumber);
 Serial.println("data sent");
 delay(1000);
}
