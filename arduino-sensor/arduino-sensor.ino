#include <SPI.h>         // needed for Arduino versions later than 0018
#include <Ethernet.h>
#include <EthernetUdp.h>         // UDP library from: bjoern@cs.stanford.edu 12/30/2008
#include <Ultrasonic.h>


// Enter a MAC address and IP address for your controller below.
// The IP address will be dependent on your local network:
byte mac[] = {
  0x90, 0xA2, 0xDA, 0x0E, 0xB6, 0x07
};
IPAddress ip(192, 168, 0, 101);
IPAddress ip_pc(192,168,0,103);

unsigned int localPort = 9876;      // local port to listen on

char  ReplyBuffer[5];
int outputTiming = 1000;

Ultrasonic sensor_frente(39, 37);

// An EthernetUDP instance to let us send and receive packets over UDP
EthernetUDP Udp;

typedef union
{
  float number;
  uint8_t bytes[4];
} FLOATUNION_t;

void setup() {
  // put your setup code here, to run once:
  Ethernet.begin(mac, ip);
  Udp.begin(localPort);
  Serial.begin(9600);
  pinMode(41,OUTPUT);
  digitalWrite(41,HIGH);
  pinMode(35,OUTPUT);
  digitalWrite(35,LOW);
}

void loop() {
  static unsigned long  loopTime = 0;
  static unsigned long  time1 = 0;
  loopTime = millis();
  
  // put your main code here, to run repeatedly:
  float dist = sensor_frente.timing();
  dist = dist / 58;
  Serial.println(dist);
  
  FLOATUNION_t myFloat;

  myFloat.number = dist;
  for(int i=0; i<4; i++)
  {
    ReplyBuffer[i] = myFloat.bytes[i];
  }

  if((loopTime - time1) >= outputTiming)
  {
    time1 = loopTime;
    Serial.println("env");
    Udp.beginPacket(ip_pc, localPort);
    Udp.write(ReplyBuffer);
    Udp.endPacket();    
  }  
}
