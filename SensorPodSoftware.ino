#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <IPAddress.h>
#include "RoveBoard.h"
#include "RoveComm.h"

RoveCommWiFiUdp RoveComm;

//////////////////////
//Software Configuration
#define USE_SENSOR_SERIAL 1  //Set to 1 to send sensor readings over Serial
#define USE_WIFI 1           //Set to 1 to use RoveComm WiFi

//////////////////////
//Pinouts
#define TEMPERATURE_PIN 5
#define MOISTURE_PIN    4

/////////////////////
//Conversion
#define TEMPERATURE_MAX_ACTUAL   1     //Analog Read
#define TEMPERATURE_MAX_MEASURED 1000  //Sensor Reading
#define TEMPERATURE_MIN_ACTUAL   0
#define TEMPERATURE_MIN_MEASURED 0

#define MOISTURE_MAX_ACTUAL      1
#define MOISTURE_MAX_MEASURED    1000
#define MOISTURE_MIN_ACTUAL      0
#define MOISTURE_MIN_MEASURED    0

int temperature_Scalar = (TEMPERATURE_MAX_ACTUAL   - TEMPERATURE_MIN_ACTUAL)/(TEMPERATURE_MAX_MEASURED - TEMPERATURE_MIN_MEASURED);              
int moisture_Scalar    = (MOISTURE_MAX_ACTUAL      - MOISTURE_MIN_ACTUAL)   /(MOISTURE_MAX_MEASURED    - MOISTURE_MIN_MEASURED);
                        

int temperature_Translational     = TEMPERATURE_MIN_ACTUAL     - (TEMPERATURE_MIN_MEASURED     * temperature_Scalar);
int moisture_Translational        = MOISTURE_MIN_ACTUAL - (MOISTURE_MIN_MEASURED * moisture_Scalar);

int temperature_Measurement;
int moisture_Measurement;
uint16_t temperature_Output;
uint16_t moisture_Output;
         

//////////////////////
//RoveComm Data Read
uint16_t data_id;
size_t   data_size;
uint8_t  data_value;

//////////////////////
//IP Address
unsigned char POD_SUBNET =  133;
char ssid[]         =  "MRDT Wifi";
char password[]     =  "Rovin2012";

//Data ID
#define TEMPERATURE_DATA_ID 1832
#define MOISTURE_DATA_ID    1833

///////////////////////////////////////////////////////////
void setup() {
  pinMode(TEMPERATURE_PIN, OUTPUT);
  pinMode(MOISTURE_PIN, OUTPUT);
  digitalWrite(TEMPERATURE_PIN, 0);
  digitalWrite(MOISTURE_PIN, 0);
  pinMode(A0, INPUT);

  if(USE_WIFI)
    RoveComm.begin(192, 168, 1, POD_SUBNET, ssid, password);

  if(USE_SENSOR_SERIAL)
    Serial.begin(9600);
  
}

void loop() {
  //Get Temperature
  digitalWrite(TEMPERATURE_PIN, 1);
  delay(100);
  temperature_Measurement = analogRead(A0);
  temperature_Output = temperature_Measurement * temperature_Scalar + temperature_Translational;
 // digitalWrite(TEMPERATURE_PIN, 0);

  delay(100);
  //Read Moisture Sensor
  digitalWrite(MOISTURE_PIN, 1);
  delay(100);
  moisture_Measurement = analogRead(A0);
  moisture_Output = moisture_Measurement * moisture_Scalar + moisture_Translational; 
  digitalWrite(MOISTURE_PIN, 0);


  //Output RoveComm
  if(USE_WIFI)
  {
    RoveComm.read(&data_id, &data_size, &data_value);
    RoveComm.write(TEMPERATURE_DATA_ID, sizeof(TEMPERATURE_DATA_ID), &temperature_Output);
    RoveComm.write(MOISTURE_DATA_ID, sizeof(MOISTURE_DATA_ID), &moisture_Output);
  }

  if(USE_SENSOR_SERIAL)
  {
    Serial.print("Temperature Ain:");
    Serial.println(temperature_Measurement);
    Serial.print("Temperature Out:");
    Serial.println(temperature_Output);
    Serial.print("Moisture Ain:");
    Serial.println(moisture_Measurement);
    Serial.print("Moisture Out:");
    Serial.println(moisture_Output);
     Serial.println("");
  }
}
