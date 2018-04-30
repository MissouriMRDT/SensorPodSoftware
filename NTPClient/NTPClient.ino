#include <ESP8266WiFi.h>
#include <WiFiUdp.h>

char ssid[] = "MRDT Wifi";  //  your network SSID (name)
char pass[] = "Rovin2012";       // your network password

typedef enum
{
  ROVE_WIFI_ERROR_SUCCESS       =  0,
  ROVE_WIFI_ERROR_UNKNOWN       = -1,
  ROVE_WIFI_ERROR_SOCKET_IN_USE = -2,
  ROVE_WIFI_ERROR_HOST_DOWN     = -3,
  ROVE_WIFI_ERROR_WOULD_BLOCK   = -4
} RoveWiFi_Error;

typedef IPAddress roveIP;
unsigned int localPort = 11000;      // local port to listen for UDP packet
const int NTP_PACKET_SIZE = 48; // NTP time stamp is in the first 48 bytes of the message
byte packetBuffer[ NTP_PACKET_SIZE]; //buffer to hold incoming and outgoing packets
// A UDP instance to let us send and receive packets over UDP
WiFiUDP udp;

#define UDP_TX_PACKET_MAX_SIZE 1500
uint8_t RoveCommBuffer[UDP_TX_PACKET_MAX_SIZE];

RoveWiFi_Error roveWiFi_GetUdpMsg(roveIP* senderIP, void* buffer, size_t bufferSize)
{
  int packetSize = udp.parsePacket();
  if (packetSize > 0) //if there is a packet available
  {
   udp.read((char*)buffer, bufferSize); // read the packet into the buffer
   //udpReceiver.read((char*)buffer, bufferSize);
    *senderIP = udp.remoteIP();
    return ROVE_WIFI_ERROR_SUCCESS;
  }
  else
  {
    return ROVE_WIFI_ERROR_WOULD_BLOCK;
  }
}

void roveComm_GetMsg(uint16_t* dataID, size_t* size, void* data) 
{
  uint8_t flags = 0;
  uint16_t seqNum = 0;
  roveIP senderIP;
  
  *dataID = 0;
  *size = 0;
  
  if (roveWiFi_GetUdpMsg(&senderIP, RoveCommBuffer, sizeof(RoveCommBuffer)) == ROVE_WIFI_ERROR_SUCCESS) 
  {
    RoveCommParseMsg(RoveCommBuffer, dataID, size, data, &seqNum, &flags);  
    Serial.println(senderIP);
    //RoveCommHandleSystemMsg(RoveCommBuffer, dataID, size, data, &seqNum, &flags, senderIP);
  }
}

static void RoveCommParseMsg(uint8_t* buffer, uint16_t* dataID, size_t* size, void* data, uint16_t* seqNum, uint8_t* flags) 
{
  int protocol_version = buffer[0];
  switch (protocol_version) 
  {
    case 1:
      *seqNum = buffer[1];
      *seqNum = (*seqNum << 8) | buffer[2];
      *flags = buffer[3];
      *dataID = buffer[4];
      *dataID = (*dataID << 8) | buffer[5];
      *size = buffer[6];
      *size = (*size << 8) | buffer[7];
      
      memcpy(data, &(buffer[8]), *size);
  }
}

void setup()
{
  Serial.begin(115200);
  delay(100);
  WiFi.mode(WIFI_STA);
  
  IPAddress ip(192,168,1,132);
  IPAddress gateway(192,168,1,1);
  IPAddress subnet(255, 255, 255, 0);
  WiFi.config(ip, gateway, subnet);
  WiFi.begin(ssid, pass);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  udp.begin(localPort);
}

void loop()
{
  delay(1000);

 /* udpReceiver.beginPacket(destIP, destPort);
  udpReceiver.write(msg, msgSize);
  udpReceiver.endPacket();
*/
  delay(1000);
  
  int cb = udp.parsePacket();
  
  if (!cb) 
  {
    Serial.println("no packet yet");
    
  } else 
  {
    Serial.print("packet received, length=");
    Serial.println(cb);

    uint16_t dataID; 
    size_t size;
    char data[256];
    
    roveComm_GetMsg(&dataID, &size, data);

    Serial.println(dataID);
    Serial.println(size);
    Serial.println((char*)data);
  }
  
  delay(1000);
}

// send an NTP request to the time server at the given address
void sendNTPpacket(IPAddress& address)
{
  Serial.println("sending NTP packet...");
  // set all bytes in the buffer to 0
  memset(packetBuffer, 0, NTP_PACKET_SIZE);
  // Initialize values needed to form NTP request
  // (see URL above for details on the packets)
  packetBuffer[0] = 0b11100011;   // LI, Version, Mode
  packetBuffer[1] = 0;     // Stratum, or type of clock
  packetBuffer[2] = 6;     // Polling Interval
  packetBuffer[3] = 0xEC;  // Peer Clock Precision
  // 8 bytes of zero for Root Delay & Root Dispersion
  packetBuffer[12]  = 49;
  packetBuffer[13]  = 0x4E;
  packetBuffer[14]  = 49;
  packetBuffer[15]  = 52;

  // all NTP fields have been given values, now
  // you can send a packet requesting a timestamp:
  udp.beginPacket(address, 123); //NTP requests are to port 123
  udp.write(packetBuffer, NTP_PACKET_SIZE);
  udp.endPacket();
}
