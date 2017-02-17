/*
This is for the arduino genuino flavor of arduino, but I would think it should compile on any of the arduino variants. 
Make sure you copy the NewPing library into your Arduino library folder. Mine is at "C:\Users\%USERNAME%\Documents\Arduino\libraries", 
but yours may be different.
To turn on the motor, you have to throw the MOTOR_PIN output to low, which is counterintuitive, 
but that seems to be the way it works. 
I plan on adding more code to log levels, alarms and other stuff to an online database/interface 
soon. 
Also, the current version has tons of garbage in it that I've commented out. That will go away soon.
*/
#include <SPI.h>
#include <WiFi.h>
#include <WiFiUdp.h>
#include <NewPing.h>
#define MOTOR_PIN 9
#define TRIGGER_PIN  7
#define ECHO_PIN     8
#define MAX_DISTANCE 500

unsigned int ping;
unsigned int distancecm;
unsigned int upperlimit=50;
unsigned int lowerlimit=100;
unsigned int alarmlimit=30;
unsigned int systemstate=0;
bool motorrunning=false;
//char* systemstates[]={"system idle ","motor running ","high alarm ","no reading alarm "};
NewPing sonar(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE);
char ssid[] = "yourNetwork";     // the name of your network
int status = WL_IDLE_STATUS;     // the Wifi radio's status
int keyIndex = 0;            // your network key Index number (needed only for WEP)

unsigned int localPort = 2390;      // local port to listen for UDP packets

IPAddress timeServer(129, 6, 15, 28); // time.nist.gov NTP server

const int NTP_PACKET_SIZE = 48; // NTP time stamp is in the first 48 bytes of the message

byte packetBuffer[ NTP_PACKET_SIZE]; //buffer to hold incoming and outgoing packets

// A UDP instance to let us send and receive packets over UDP
WiFiUDP Udp;

void setup() {
  // put your setup code here, to run once:
Serial.begin(115200);
while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
pinMode(MOTOR_PIN, OUTPUT);
digitalWrite(MOTOR_PIN,HIGH);
  

    // check for the presence of the shield:
  if (WiFi.status() == WL_NO_SHIELD) {
    Serial.println("WiFi shield not present"); 
    // don't continue:
    while(true);
  } 
  
  String fv = WiFi.firmwareVersion();
if (fv != "1.1.0") 
{
Serial.println("Please upgrade the firmware");
}
 // attempt to connect to Wifi network:
  while ( status != WL_CONNECTED) { 
    Serial.print("Attempting to connect to open SSID: ");
    Serial.println(ssid);
    status = WiFi.begin(ssid);
    
// wait 10 seconds for connection:
delay(10000);
}
}

void printWifiData() {
  // print your WiFi shield's IP address:
  IPAddress ip = WiFi.localIP();
    Serial.print("IP Address: ");
  Serial.println(ip);
  Serial.println(ip);
  
  // print your MAC address:
  byte mac[6];  
  WiFi.macAddress(mac);
  Serial.print("MAC address: ");
  Serial.print(mac[5],HEX);
  Serial.print(":");
  Serial.print(mac[4],HEX);
  Serial.print(":");
  Serial.print(mac[3],HEX);
  Serial.print(":");
  Serial.print(mac[2],HEX);
  Serial.print(":");
  Serial.print(mac[1],HEX);
  Serial.print(":");
  Serial.println(mac[0],HEX);
  
  // print your subnet mask:
  IPAddress subnet = WiFi.subnetMask();
  Serial.print("NetMask: ");
  Serial.println(subnet);

  // print your gateway address:
  IPAddress gateway = WiFi.gatewayIP();
  Serial.print("Gateway: ");
  Serial.println(gateway);
}

void printCurrentNet() {
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print the MAC address of the router you're attached to:
  byte bssid[6];
  WiFi.BSSID(bssid);    
  Serial.print("BSSID: ");
  Serial.print(bssid[5],HEX);
  Serial.print(":");
  Serial.print(bssid[4],HEX);
  Serial.print(":");
  Serial.print(bssid[3],HEX);
  Serial.print(":");
  Serial.print(bssid[2],HEX);
  Serial.print(":");
  Serial.print(bssid[1],HEX);
  Serial.print(":");
  Serial.println(bssid[0],HEX);

  // print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.println(rssi);

  // print the encryption type:
  byte encryption = WiFi.encryptionType();
  Serial.print("Encryption Type:");
  Serial.println(encryption,HEX);
}

void loop() {

  // put your main code here, to run repeatedly:
  
 sendNTPpacket(timeServer); // send an NTP packet to a time server
  // wait to see if a reply is available
  delay(1000);
  if (Udp.parsePacket()) {
    Serial.println("packet received");
    // We've received a packet, read the data from it
    Udp.read(packetBuffer, NTP_PACKET_SIZE); // read the packet into the buffer

    //the timestamp starts at byte 40 of the received packet and is four bytes,
    // or two words, long. First, esxtract the two words:

    unsigned long highWord = word(packetBuffer[40], packetBuffer[41]);
    unsigned long lowWord = word(packetBuffer[42], packetBuffer[43]);
    // combine the four bytes (two words) into a long integer
    // this is NTP time (seconds since Jan 1 1900):
    unsigned long secsSince1900 = highWord << 16 | lowWord;
    Serial.print("Seconds since Jan 1 1900 = ");
    Serial.println(secsSince1900);

    // now convert NTP time into everyday time:
    Serial.print("Unix time = ");
    // Unix time starts on Jan 1 1970. In seconds, that's 2208988800:
    const unsigned long seventyYears = 2208988800UL;
    // subtract seventy years:
    unsigned long epoch = secsSince1900 - seventyYears;
    // print Unix time:
    Serial.println(epoch);


    // print the hour, minute and second:
    Serial.print("The UTC time is ");       // UTC is the time at Greenwich Meridian (GMT)
    Serial.print((epoch  % 86400L) / 3600); // print the hour (86400 equals secs per day)
    Serial.print(':');
    if (((epoch % 3600) / 60) < 10) {
      // In the first 10 minutes of each hour, we'll want a leading '0'
      Serial.print('0');
    }
    Serial.print((epoch  % 3600) / 60); // print the minute (3600 equals secs per minute)
    Serial.print(':');
    if ((epoch % 60) < 10) {
      // In the first 10 seconds of each minute, we'll want a leading '0'
      Serial.print('0');
    }
    Serial.println(epoch % 60); // print the second
  }
  // wait ten seconds before asking for the time again
  delay(10000);
}

// send an NTP request to the time server at the given address
unsigned long sendNTPpacket(IPAddress& address) {
  //Serial.println("1");
  // set all bytes in the buffer to 0
  memset(packetBuffer, 0, NTP_PACKET_SIZE);
  // Initialize values needed to form NTP request
  // (see URL above for details on the packets)
  //Serial.println("2");
  packetBuffer[0] = 0b11100011;   // LI, Version, Mode
  packetBuffer[1] = 0;     // Stratum, or type of clock
  packetBuffer[2] = 6;     // Polling Interval
  packetBuffer[3] = 0xEC;  // Peer Clock Precision
  // 8 bytes of zero for Root Delay & Root Dispersion
  packetBuffer[12]  = 49;
  packetBuffer[13]  = 0x4E;
  packetBuffer[14]  = 49;
  packetBuffer[15]  = 52;

  //Serial.println("3");

  // all NTP fields have been given values, now
  // you can send a packet requesting a timestamp:
  Udp.beginPacket(address, 123); //NTP requests are to port 123
  //Serial.println("4");
  Udp.write(packetBuffer, NTP_PACKET_SIZE);
  //Serial.println("5");
  Udp.endPacket();
  //Serial.println("6");
  
  //This begins the sonar ping code.
  ping=sonar.ping_median(200);
  distancecm=(sonar.convert_cm(ping));

/*If the distance to the liquid is less than the alarm limit and not equal to 0(meaning no distance returned from sonar), 
  turn on the pump, print some messages on serial and set the motorrunning variable to true.
  */
if ((distancecm<alarmlimit)&&(distancecm!=0))
  {
    digitalWrite(MOTOR_PIN,LOW);
    Serial.print("Motor is running");
    Serial.print(",High Alarm");
    motorrunning=true;
    //systemstate=2;
    
  }
/*
If the distance to the liquid is less than the upperlimit set point, not equal to 0(error condition) and the motorrunning variable is
false, turn the motor on, print a serial message and set motorrunning variable to true.
*/  
  else if ((distancecm<upperlimit)&&(distancecm!=0)&&(motorrunning==false))
  {
    digitalWrite(MOTOR_PIN,LOW);
    Serial.print("Motor is turning on");
    motorrunning=true;
    //systemstate=1;
  }
  
/*
If the distance to the liquid is less than the lowerlimit set point, not equal to 0(error condition) and the motorrunning variable is
true, print a serial message and set motorrunning variable to true.
*/  
   else if ((distancecm<lowerlimit)&&(distancecm!=0)&&(motorrunning==true))
  {
    //digitalWrite(MOTOR_PIN,LOW);
    Serial.print("Motor is running-Normal");
    motorrunning=true;
    //systemstate=1;
  }
  
/*
If the distance to the liquid is more than the upperlimit set point, not equal to 0(error condition) and the motorrunning variable is
true, Turn the motor off, print a serial message and set motorrunning variable to false.
*/ 
  else if ((distancecm>upperlimit)&&(distancecm!=0)&&(motorrunning==true))
  {
    digitalWrite(MOTOR_PIN,HIGH);
   Serial.print("Motor is turning off");
   motorrunning=false;
    //systemstate=0;
  }
  
  /*
If the distance is equal to 0(error condition), Turn the motor off, print a serial message and set motorrunning variable to false.
*/ 
  else if (ping==0) 
  {
    Serial.print("No reading!");
    digitalWrite(MOTOR_PIN,HIGH);
   Serial.print("Motor is not running");
   motorrunning=false;
    //systemstate=3;
  }
  
  /*
If the distance to the liquid is more than the upperlimit set point, not equal to 0(error condition) and the motorrunning variable is
false, print a serial message.
*/ 
 else if ((distancecm>upperlimit)&&(distancecm!=0)&&(motorrunning==false))
  {
    //digitalWrite(MOTOR_PIN,LOW);
    Serial.print("IDLE-Motor is not running");
    //systemstate=1;
  }
  
  //The logic should never get here. If it has, I've missed a possibility.
  else
  {
    Serial.print("Nothing to see here.");
    }
 // Serial.print(systemstates[systemstate]);
  /*Serial.print(",motor is ");
  if (motorrunning==true)
  {
    Serial.print("running.");
  }
 else if (motorrunning==false) 
  {
    Serial.print("not running.");
  }
 else
 {
  Serial.print("No idea");
 }
  */
  //Serial.print(motorrunning);
  //if ((sonar.convert_cm(ping)<alarmlimit)&&(sonar.convert_cm(ping)!=0)) Serial.print("alarm ");
  //if ((sonar.convert_cm(ping)<upperlimit)&&(sonar.convert_cm(ping)!=0)) Serial.print("motor stop ");
  //if ((sonar.convert_cm(ping)>lowerlimit)&&(sonar.convert_cm(ping)!=0)) Serial.print("motor start ");
  //if (sonar.convert_cm(ping)==0) Serial.print("sensor error ");
  Serial.print(",Ping: ");
  Serial.print(sonar.convert_cm(ping));
  Serial.println("cm");
 // digitalWrite(MOTOR_PIN,LOW);

  // you're connected now, so print out the data:
  Serial.print("You're connected to the network");
  printCurrentNet();
  printWifiData();
}

