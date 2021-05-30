#include <AFMotor.h>
#include <SPI.h>         // needed for Arduino versions later than 0018
#include <Ethernet.h>
#include <EthernetUdp.h>         // UDP library from: bjoern@cs.stanford.edu 12/30/2008

// Enter a MAC address and IP address for your controller below.
byte mac[] = {0x90,0xA2,0xDA,0x0E,0xB6,0x06
};
// The IP address will be dependent on your local network:
IPAddress ip(192,168,0,102);
int localPort = 9876;      // local port to listen on
float anterior = 2;

char packetBuffer[4];  //buffer to hold incoming packet, [UDP_TX_PACKET_MAX_SIZE]

EthernetUDP Udp;

AF_DCMotor motor(1); //Seleciona o motor 1
 
int pinBotao = 40; //Declara pino do botao
bool fail = false; //caso erro de comunicacao ir para estado seguro
int outputTiming = 10000;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(A15, INPUT); //potenciometro
  pinMode(pinBotao,INPUT); //botao digital
  motor.setSpeed(255);
  motor.run(RELEASE);
  Ethernet.begin(mac, ip);
  Udp.begin(localPort);
  Serial.println("Start");
}

void loop() {
  // put your main code here, to run repeatedly:
  static unsigned long  loopTime = 0;
  static unsigned long  time1 = 0;
  loopTime = millis();
  float potenciometro = analogRead(A15); // valor potenciometro para controle
  int estado_botao = digitalRead(pinBotao);
  int packetSize = Udp.parsePacket();
  Udp.read(packetBuffer, 4);

  float pwm;
  pwm = *(float*)packetBuffer;

  if((loopTime - time1) >= outputTiming)
  {
    time1 = loopTime;
    Serial.println("entrei no safe");
    Serial.println(anterior);
    Serial.println(pwm);
    if ((anterior - pwm) == 0)
    {
      if ((pwm != 0)||(pwm!=255))
      {
        fail = true;        
      }
    }
    anterior = pwm;
  }
  
  if (estado_botao == HIGH) 
  {
    fail = false;
    anterior = 0;
    //CONTROLE MANUAL
    pwm = map(potenciometro, 0, 1023, 0, 255);
    if (pwm > 255)
    {
      pwm = 255;
    }
    else if (pwm < 0)
    {
      pwm = 0;
    }
    Serial.println("Controle Manual");
    Serial.println(pwm);
    motor.setSpeed(pwm);
    motor.run(FORWARD);
  }
  else
  {
    if (fail == false)
    {
      //ATUAÇÃO DO SISTEMA DE REDE
      //Serial.println("Controle Remoto"); 
      //Serial.println(pwm);
      motor.setSpeed(pwm);
      motor.run(FORWARD);    
    }
  }
    
  if (fail == true) // FAIL SAFE
  {
    Serial.println("FAIL");
    motor.setSpeed(0);
    motor.run(RELEASE);
  }
}
