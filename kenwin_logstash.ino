   /*

#  Electricity Web Service Project
#  Copyright 2018 raja <raja@raja-Inspiron-N5110>
#  
#  This program is free software; you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation; either version 2 of the License, or
#  (at your option) any later version.
#  
#  This program is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#  
#  You should have received a copy of the GNU General Public License
#  along with this program; if not, write to the Free Software
#  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
#  MA 02110-1301, USA.
 *  
 */
#include <ESP8266WiFi.h>
#include <Wire.h>
//#include <Adafruit_Sensor.h>
//#include <Adafruit_BMP085_U.h>
#include "TimeLib.h"
#include <WiFiUdp.h>
static const char ntpServerName[] = "us.pool.ntp.org";  
// Setup your wifi SSID and password here.
const char* ssid     = "Xiaomi_0258";
const char* password = "@ex220o!";
const int timeZone = 0;  // UTC
// Variables needed for NTP
// Elasticsearch needs us to generate timestamps for the data in order to make date histograms in Kibana.
WiFiUDP Udp;
unsigned int localPort = 8888;  // local port to listen for UDP packets
time_t getNtpTime();
void printDigits(int digits);
void sendNTPpacket(IPAddress &address);
// This is the IP address, or DNS name of my Elasticsearch instance.
//const char* host = "35.202.123.96";
const char* host = "192.168.31.224"; //raja-Inspiron-N5110
//const char* host = "35.196.176.163"; //raja-Inspiron-N5110
const int port = 9600;
int motion;
// Variables
float temperature;
String timestamp;
time_t start_time;
uint32_t t_ms;
uint32_t start_mills;
String run_mills;
int milis_chars;
String mac;
void setup() {
  Serial.begin(115200);
  delay(100);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");  
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  mac = WiFi.macAddress();
  mac = String(mac);
  mac.replace(":","");
  Serial.println(mac);
  Serial.println("Setting up NTP");
  Udp.begin(localPort);
  Serial.print("Local port: ");
  Serial.println(Udp.localPort());
  Serial.println("waiting for sync");
  setSyncProvider(getNtpTime);
  setSyncInterval(300);
  start_time = now();
  Serial.println("Welcome to Kenwin Web Service"); Serial.println("");
  
}
void loop() {    
  
  float Voltage = 110.00;
  float Current = 12;
  float AmpereHour= 14;
  String Dafaq;
  // Use WiFiClient class to create TCP connections, connect to the Elasticsearch instance.
  WiFiClient client;
  ;
  Serial.println( client.connect(host, port));
  if (!client.connect(host, port)) {
    Serial.println("connection failed");
    return;
  }
  run_mills = String(millis());
  milis_chars = run_mills.length();
  // To generate a millisecond unix timestamp, we first get the second timestamp, and add to it, the last three characters of the arduino/relative millisecond timestamp
  timestamp = String(now()) + run_mills.charAt(milis_chars-3) + run_mills.charAt(milis_chars-2) + run_mills.charAt(milis_chars-1);
  // With such a simple document, we're just going to use a string to generate the JSON to send to Elasticsearch
  //String data = "{\"Voltage\": "+String(Voltage)+", \"Current\": "+String(Current)+", \"AmpereHour\": "+String(AmpereHour)+", \"timestamp\": "+ timestamp +"}";
  String data = "{\"Voltage\": "+String(Voltage)+", \"uname\": \""+String(mac)+"\", \"Current\": "+String(Current)+", \"AmpereHour\": "+String(AmpereHour)+", \"timestamp\": "+ timestamp +"}";
  // We can inspect the data being sent over the Serial line, in the Arduino IDE.
 // Serial.println(data);
//  String data2 = '\n{\n    "query" : {\n        "match_all": { "q": { "*": "*"}\n    }\n }}';
  // We now create a URI for the request
  // This is the index of the Elasticsearch document we're creating
  String url = "/kenwin/reading";
  // 
 /* client.print(String("POST ") + url + " HTTP/1.1\r\n" +
               // If you're using Shield, you'll need to generate an authentication header
               "Content-Type: application/json" + "Content-Length: " + data.length() + "\r\n" +
               "\r\n" + data); */
    Dafaq =( String("POST ") + url + " HTTP/1.1\r\n" +
               // If you're using Shield, you'll need to generate an authentication header
               "Content-Type: application/json" + "\r\n" + "Content-Length: " + data.length() + "\r\n" +
               "\r\n" + data+ "\r\n"+ "Connection: keep-alive\r\n\r\n");
               Serial.println(Dafaq);
   Serial.println(client.print(String("POST ") + url + " HTTP/1.1\r\n" +
               // If you're using Shield, you'll need to generate an authentication header
               "Content-Type: application/json" + "\r\n" + "Content-Length: " + data.length() + "\r\n" +
               "\r\n" + data));
  // We need this delay in here to give the WiFi Time
  delay(50);

//client.print(String("GET ") + url + " HTTP/1.1\r\n" + "Host:" + "35.202.123.96" + "\r\n"
               // If you're using Shield, you'll need to generate an authentication header
              // "Content-Type: application/json");
//  delay(50);
  // Read all the lines of the reply from server and print them to Serial
  while(client.available()){
    String line = client.readString();
    Serial.print(line);
    //Serial.println("I am inside response");
  }
  Serial.println();
}
/* Copied from https://github.com/PaulStoffregen/Time/blob/master/examples/TimeNTP_ESP8266WiFi/TimeNTP_ESP8266WiFi.ino#L99 */
/*-------- NTP code ----------*/
const int NTP_PACKET_SIZE = 48; // NTP time is in the first 48 bytes of message
byte packetBuffer[NTP_PACKET_SIZE]; //buffer to hold incoming & outgoing packets
time_t getNtpTime()
{
  IPAddress ntpServerIP; // NTP server's ip address
  while (Udp.parsePacket() > 0) ; // discard any previously received packets
  Serial.println("Transmit NTP Request");
  // get a random server from the pool
  WiFi.hostByName(ntpServerName, ntpServerIP);
  Serial.print(ntpServerName);
  Serial.print(": ");
  Serial.println(ntpServerIP);
  sendNTPpacket(ntpServerIP);
  uint32_t beginWait = millis();
  while (millis() - beginWait < 1500) {
    int size = Udp.parsePacket();
    if (size >= NTP_PACKET_SIZE) {
      Serial.println("Receive NTP Response");
      Udp.read(packetBuffer, NTP_PACKET_SIZE);  // read packet into the buffer
      unsigned long secsSince1900;
      // convert four bytes starting at location 40 to a long integer
      secsSince1900 =  (unsigned long)packetBuffer[40] << 24;
      secsSince1900 |= (unsigned long)packetBuffer[41] << 16;
      secsSince1900 |= (unsigned long)packetBuffer[42] << 8;
      secsSince1900 |= (unsigned long)packetBuffer[43];
      return secsSince1900 - 2208988800UL + timeZone * SECS_PER_HOUR;
    }
  }
  Serial.println("No NTP Response :-(");
  return 0; // return 0 if unable to get the time
}
// send an NTP request to the time server at the given address
void sendNTPpacket(IPAddress &address)
{
  // set all bytes in the buffer to 0
  memset(packetBuffer, 0, NTP_PACKET_SIZE);
  // Initialize values needed to form NTP request
  // (see URL above for details on the packets)
  packetBuffer[0] = 0b11100011;   // LI, Version, Mode
  packetBuffer[1] = 0;     // Stratum, or type of clock
  packetBuffer[2] = 6;     // Polling Interval
  packetBuffer[3] = 0xEC;  // Peer Clock Precision
  // 8 bytes of zero for Root Delay & Root Dispersion
  packetBuffer[12] = 49;
  packetBuffer[13] = 0x4E;
  packetBuffer[14] = 49;
  packetBuffer[15] = 52;
  // all NTP fields have been given values, now
  // you can send a packet requesting a timestamp:
  Udp.beginPacket(address, 123); //NTP requests are to port 123
  Udp.write(packetBuffer, NTP_PACKET_SIZE);
  Udp.endPacket();
}
