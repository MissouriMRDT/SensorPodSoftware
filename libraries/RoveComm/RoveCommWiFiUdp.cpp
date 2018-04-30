 /*
  RoveCommWiFiUdp.h - Library for Wiring + Processing IDE's (Energia/Arduino) 
  
  Copyright (c) 2018 Missouri S&T Mars Rover Design Team.  All right reserved.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

#include <stdint.h>
#include <stddef.h>

#include "RoveComm.h"
#include "RoveCommProtocol.h"

////////////////////////////////////////////////////////////////////////////
// If => Texas Instruments WiFi (CC3200 / CC3100)
//
// Board Manager installs to => C:\Users\<YourUser>\AppData\Local\Energia15
//
// #include <Energia.h>
// #include <SPI.h>
// #include <WiFi.h>
// #include <WiFiUdp.h>

////////////////////////////////////////////////////////////////////////////
// Else => Arduino WiFi (Esp8266)
//
// Board Manager installs to =>  C:\Users\<YourUser>\AppData\Local\Arduino15
//
// #include <Arduino.h>
// #include <SPI.h>
// #include <ESP8266WiFi.h>
// #include <WiFiUdp.h>

#ifdef Energia_h
  #include <Energia.h>
#else
  #include <Arduino.h>
#endif 
#include <SPI.h>
//#ifdef ARDUINO_ESP8266_RELEASE
  #include <ESP8266WiFi.h> 
//#else
//  #include <WiFi.h>
//#endif 
#include <WiFiUdp.h> // ESP8266WiFi WiFi; => instance already in ESP8266WiFi.cpp
   
WiFiUDP     WiFiUdp;     

const IPAddress NULL_IP(0, 0, 0, 0);

IPAddress RoveCommWiFiUdpSubscribers[ROVECOMM_WIFI_UDP_SUBSCRIBERS];

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// todo   
// todo 
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
 
void RoveCommWiFiUdp::begin(uint8_t ip_octet_1, uint8_t ip_octet_2, uint8_t ip_octet_3, uint8_t ip_octet_4, char* ssid, char* password) 
{ 
  int  i;
  
  for (i=0; i < ROVECOMM_WIFI_UDP_SUBSCRIBERS; i++) 
  {
    RoveCommWiFiUdpSubscribers[i] = NULL_IP;
  }
  
  IPAddress LocalIp(  ip_octet_1, ip_octet_2, ip_octet_3, ip_octet_4); 
  IPAddress GatewayIp(ip_octet_1, ip_octet_2, ip_octet_3, 1         );
  IPAddress SubnetIp( 255,        255,        255,        0         ); 

 // #ifdef ARDUINO_ESP8266_RELEASE
	WiFi.mode(WIFI_STA);
 // #endif
  
  WiFi.config(LocalIp, GatewayIp, SubnetIp);
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED)
  {
	  delay(100);
  }
  
  
  WiFiUdp.begin(ROVECOMM_WIFI_UDP_PORT); 
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// todo   
// todo 
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
 
void RoveCommWiFiUdp::write(uint16_t data_id, size_t data_size, const void* data) 
{ 
  uint8_t rovecomm_packet[ROVECOMM_PACKET_HEADER_SIZE + data_size]; 
  
  RoveComm_packPacket(rovecomm_packet, data_id, data_size, (const uint8_t*)data);
  
  int  i;
  for (i=0; i < ROVECOMM_WIFI_UDP_SUBSCRIBERS; i++) 
  {
    if (!(RoveCommWiFiUdpSubscribers[i] == NULL_IP)) 
    {       
      WiFiUdp.beginPacket(RoveCommWiFiUdpSubscribers[i], ROVECOMM_ETHERNET_UDP_PORT);
      WiFiUdp.write(rovecomm_packet, ROVECOMM_PACKET_HEADER_SIZE + data_size);
      WiFiUdp.endPacket();
    }
  } 
}

void RoveCommWiFiUdp::writeTo(uint16_t data_id, size_t  data_size, const void* data, uint8_t ip_octet_1, uint8_t ip_octet_2, uint8_t ip_octet_3, uint8_t ip_octet_4, uint16_t port) 
{ 
  uint8_t rovecomm_packet[ROVECOMM_PACKET_HEADER_SIZE + data_size];
  
  RoveComm_packPacket(rovecomm_packet, data_id, data_size, (const uint8_t*)data);

  IPAddress RemoteIp(ip_octet_1, ip_octet_2, ip_octet_3, ip_octet_4); 
  
  WiFiUdp.beginPacket(RemoteIp, port);
  WiFiUdp.write(rovecomm_packet, ROVECOMM_PACKET_HEADER_SIZE + data_size);
  WiFiUdp.endPacket();  
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// todo todo todo todo
// todo todo todo todo
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void RoveCommWiFiUdp::read(uint16_t*  data_id, size_t* data_size, void* data) 
{ 
  *data_id   = 0; 
  *data_size = 0; 
  
  int packet_size = WiFiUdp.parsePacket();  
  if (packet_size > 0)
  {       
    uint8_t rovecomm_packet[packet_size];    
    WiFiUdp.read(rovecomm_packet, packet_size);     
    IPAddress RemoteIp = WiFiUdp.remoteIP();
    
    uint8_t rovecomm_version; 
    
    RoveComm_unpackPacket(rovecomm_packet, &rovecomm_version, data_id, data_size, (uint8_t*)data); 
    
    if (rovecomm_version != 1) 
    {
      *data_id   = ROVECOMM_VERSION_ERROR;
      *data_size = 0; 
      
    } else if  (*data_id == ROVECOMM_SUBSCRIBE_REQUEST)
    {
      int  i;
      for (i=0; i < ROVECOMM_WIFI_UDP_SUBSCRIBERS; i++) 
      {
        if (RoveCommWiFiUdpSubscribers[i] == RemoteIp) 
        {
          break; // already subscribed         
        } else if (RoveCommWiFiUdpSubscribers[i] == NULL_IP) 
        {
          RoveCommWiFiUdpSubscribers[i] = RemoteIp; // add subscriber
          break;
        }
      } 
        
    } else if (*data_id == ROVECOMM_UNSUBSCRIBE_REQUEST) 
    {
      int  i;
      for (i=0; i < ROVECOMM_WIFI_UDP_SUBSCRIBERS; i++) 
      {
        if (RoveCommWiFiUdpSubscribers[i] == RemoteIp) 
        {
          RoveCommWiFiUdpSubscribers[i] = NULL_IP; // remove subscriber
          break;        
        }
      }
    }  
  } 
}
/*
void RoveCommWiFiUdp::readFrom(uint16_t data_id, size_t data_size, const void* data, uint8_t* ip_octet_1, uint8_t* ip_octet_2, uint8_t* ip_octet_3, uint8_t* ip_octet_4)
{ 
  *data_id    = 0; 
  *data_size  = 0;
  *ip_octet_1 = 0; 
  *ip_octet_2 = 0;
  *ip_octet_3 = 0;
  *ip_octet_4 = 0; 
  
  int packet_size = WiFiUdp.parsePacket();  
  if (packet_size > 0)
  {       
    uint8_t rovecomm_packet[packet_size];    
    WiFiUdp.read(rovecomm_packet, packet_size);     
    IPAddress RemoteIp = WiFiUdp.remoteIP();

    *ip_octet_1 = RemoteIp[0];
    *ip_octet_2 = RemoteIp[1];
    *ip_octet_3 = RemoteIp[2]; 
    *ip_octet_4 = RemoteIp[0];
    
    uint8_t rovecomm_version; 
    
    RoveComm_unpackPacket(rovecomm_packet, &rovecomm_version, data_id, data_size, (uint8_t*)data); 
    
    if (rovecomm_version != 1) 
    {
      *data_id   = ROVECOMM_VERSION_ERROR;
      *data_size = 0; 
      
    } else if  (*data_id == ROVECOMM_SUBSCRIBE_REQUEST)
    {
      int  i;
      for (i=0; i < ROVECOMM_WIFI_UDP_SUBSCRIBERS; i++) 
      {
        if (WiFiUdpSubscribers[i] == RemoteIp) 
        {
          break; // already subscribed         
        } else if (WiFiUdpSubscribers[i] == NULL_IP) 
        {
          WiFiUdpSubscribers[i] = RemoteIp; // add subscriber
          break;
        }
      }
        
    } else if (*data_id == ROVECOMM_UNSUBSCRIBE_REQUEST) 
    {
      int  i;
      for (i=0; i < ROVECOMM_WIFI_UDP_SUBSCRIBERS; i++) 
      {
        if (WiFiUdpSubscribers[i] == RemoteIp) 
        {
          WiFiUdpSubscribers[i] = NULL_IP; // remove subscriber
          break;        
        }
      }
    }  
  } 
}
*/
