#include "RoveComm.h"

RoveCommEthernetUdp RoveComm;

uint16_t data_id; 
size_t   data_size; 
uint8_t  data[256];

void setup() 
{
  RoveComm.begin(192, 168, 1, 2); 
  Serial.begin(9600);
}

void loop() 
{
  RoveComm.read(&data_id, &data_size, data);

  delay(100);
  
  RoveComm.write(data_id, data_size, data);

  delay(100);

  if(data_size > 0)
  {
    Serial.print("data_id: ");
    Serial.println(data_id);
    Serial.print("data_size: ");
    Serial.println(data_size);
    Serial.print("data: ");
  
    int i;
    for(i=0; i < data_size; i++)
    {
      Serial.print("0x");
      Serial.print(data[i], HEX);
      Serial.print(", ");   
    }
  
    Serial.println("");
 }
   
}
