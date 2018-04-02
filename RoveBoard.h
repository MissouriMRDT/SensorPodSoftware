// Author: Gbenga Osibodu

#ifndef ROVEWIFI_H_
#define ROVEWIFI_H_

#include <stdint.h>
#include <stddef.h>
#include <IPAddress.h>

//#include "supportingUtilities/IPAddress.h"

typedef enum
{
  ROVE_WIFI_ERROR_SUCCESS       =  0,
  ROVE_WIFI_ERROR_UNKNOWN       = -1,
  ROVE_WIFI_ERROR_SOCKET_IN_USE = -2,
  ROVE_WIFI_ERROR_HOST_DOWN     = -3,
  ROVE_WIFI_ERROR_WOULD_BLOCK   = -4
} RoveWiFi_Error;

typedef IPAddress roveIP;

#define ROVE_IP_ADDR_NONE INADDR_NONE

//Start up the ethernet hardware and assign an IP to this board. Must be called before anything else
extern void roveWiFi_NetworkingStart(char* ssid, char* password, uint8_t IP_octet1, uint8_t IP_octet2, uint8_t IP_octet3, uint8_t IP_octet4);

//Start listening for UDP messages on a given port. Must be called before GetUdpMsg can be called.
extern RoveWiFi_Error roveWiFi_UdpSocketListen(uint16_t port);

//Sends a udp message to the destination IP and port
extern RoveWiFi_Error roveWiFi_SendUdpPacket(roveIP destIP, uint16_t destPort, const uint8_t* msg, size_t msgSize);

//Checks to see if we've received a udp message since the last time this function was called and returns it if we did.
//Returns-by-pointer the IP of the sending device, and fills up an array that the user must set up themselves
//with the udp message's bytes, up to the size of the users buffer or less.
//Returns Success if there was a udp message in our receive buffer.
extern RoveWiFi_Error roveWiFi_GetUdpMsg(roveIP* senderIP, void* buffer, size_t bufferSize);

//attach a function to be ran automatically whenever a udp packet is received.
//function arguments:   msgBuffer[]: an array of size msgSize that contains all the data bytes that was
//                                   in the udp packet we just received
//                      msgSize:     The size of the data array
//
//function returns:     whether or not to keep this packet in the buffer. If you return true, roveWifi will keep it in its
//                      internal udp message buffer so that it will appear again when you call GetUdpMsg, after you eventually
//                      process enough packets to bring this one to the top. False means we'll remove the packet from the
//                      buffer when the function returns and you won't see it again
//
//Note:                 Comparing to getUdpMsg: GetUdpMsg gets you the the message at the top of the buffer, this will get you the message
//                      at the bottom of the buffer. GetUdpMsg automatically removes the read packet from the buffer, this will let you choose
//                      to put it back into the buffer or remove it
extern void roveWifi_attachUdpReceiveCb(bool (*userFunc)(uint8_t* msgBuffer, size_t msgSize));




#endif
