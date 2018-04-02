// RoveBoard.cpp for Esp8266
// Author: Andrew Van Horn (But mostly Judah)

#include "RoveBoard.h"
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <IPAddress.h>

WiFiUDP udpReceiver;

void roveWiFi_NetworkingStart(char* ssid, char* password, uint8_t IP_octet1, uint8_t IP_octet2, uint8_t IP_octet3, uint8_t IP_octet4)
{
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
  }
  IPAddress ip(IP_octet1, IP_octet2, IP_octet3, IP_octet4);
  IPAddress gateway(IP_octet1, IP_octet2, IP_octet3, 1);
  IPAddress subnet(255, 255, 255, 0);
  WiFi.config(ip, gateway, subnet);
}

RoveWiFi_Error roveWiFi_UdpSocketListen(uint16_t port)
{
  udpReceiver.begin(port);
  return ROVE_WIFI_ERROR_SUCCESS;
}

RoveWiFi_Error roveWiFi_SendUdpPacket(roveIP destIP, uint16_t destPort, const uint8_t* msg, size_t msgSize)
{
  udpReceiver.beginPacket(destIP, destPort);
  udpReceiver.write(msg, msgSize);
  udpReceiver.endPacket();
  return ROVE_WIFI_ERROR_SUCCESS;
}

RoveWiFi_Error roveWiFi_GetUdpMsg(roveIP* senderIP, void* buffer, size_t bufferSize)
{
  int packetSize = udpReceiver.parsePacket(); 
  
  if (packetSize > 0) //if there is a packet available
  {
   udpReceiver.read((char*)buffer, bufferSize);
    *senderIP = udpReceiver.remoteIP();
    return ROVE_WIFI_ERROR_SUCCESS;
  }
  else
  {
    return ROVE_WIFI_ERROR_WOULD_BLOCK;
  }
}

void wait(int micros) {
  delayMicroseconds(micros);
}
