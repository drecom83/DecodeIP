#include <ESP8266WiFi.h>          //ESP8266 Core WiFi Library (you most likely already have this in your sketch)
#include "decodeip.h"

/////////////////////
// Pin Definitions //
/////////////////////
// On a ESP8266-12 GPIO0 is used, physical name is pin D0
// On a ESP8266-12 GPIO5 is used, physical name is pin D5

const uint8_t IP_ACK = D4;
const uint8_t IP_PIN0 = D3;
const uint8_t IP_PIN1 = D2;
const uint8_t IP_PIN2 = D1;
const uint8_t IP_BUTTON = D7;
String ip4 = "192.168.004.001";
bool resetIP = false;
bool buttonInterruptFlag = false;

/* paramer for showBlock */
const uint8_t MAXSHOW = 10;
volatile  uint8_t maxShowTime = MAXSHOW;

/* count the number of button-presses within a choosen IP-block */
volatile uint8_t pressCount = 0;

/* holds the time in millis between button-press and button-release */
uint32_t buttonPressTime = 0;

/* time that the button was pressed, in milis since esp8266 was powered */
/* TODO: act on overload */
volatile uint32_t pressTime;


DecodeIP * decodeIP = new DecodeIP(IP_ACK, IP_PIN0, IP_PIN1, IP_PIN2, IP_BUTTON);
void ICACHE_RAM_ATTR detectButton();
void buttonInterruptOn();
void buttonInterruptOff();


void delayInMillis(uint8_t ms) {
  for (uint16_t i = 0; i <= ms; i++)
  {
    delayMicroseconds(250);   // delay in the loop could cause an exception (9) when using interrupts
    delayMicroseconds(250);   // delay in the loop could cause an exception (9) when using interrupts
    delayMicroseconds(250);   // delay in the loop could cause an exception (9) when using interrupts
    delayMicroseconds(250);   // delay in the loop could cause an exception (9) when using interrupts
  }
}

void ICACHE_RAM_ATTR detectButton() {  // ICACHE_RAM_ATTR is voor interrupts
  // this function is called after a change of pressed button  
  buttonInterruptOff();  // to prevent exception

  delayInMillis(10);      // prevent bounce
  buttonInterruptFlag = true;
  
  digitalWrite(IP_ACK, LOW);
  digitalWrite(IP_PIN0, LOW);
  digitalWrite(IP_PIN1, LOW);
  digitalWrite(IP_PIN2, LOW);

  if (digitalRead(IP_BUTTON) == HIGH)
  {
    // counter within one of the four IP-blocks
    pressCount++;
    maxShowTime = 0;                // counter for repeating flashresult, until this-MAXSHOW
    pressTime = millis();           // start of pressing the button
  }
  else
  {
    buttonPressTime = millis() - pressTime;
  }
  buttonInterruptOn();  // to prevent exception
}

void buttonInterruptOn() {
  //attachInterrupt(digitalPinToInterrupt(IR_RECEIVE_1), detectPulse, CHANGE);
  // 0 = ir_light, 1 is no ir_light
  attachInterrupt(digitalPinToInterrupt(IP_BUTTON), detectButton, CHANGE);
}

void buttonInterruptOff() {
  detachInterrupt(IP_BUTTON);
}

void initHardware()
{
  Serial.begin(115200);

  pinMode(IP_ACK, OUTPUT);    // default down
  pinMode(IP_PIN0, OUTPUT);   // default down
  pinMode(IP_PIN1, OUTPUT);   // default down
  pinMode(IP_PIN2, OUTPUT);   // default down
  pinMode(IP_BUTTON, INPUT);  // default down
}

void setup()
{
  /* It seems to help preventing ESPerror messages with mode(3,6) when
  using a delay between setup parts */
  initHardware();

        //String ip4 = "192.168.004.001";
        //decodeIP->process(ip4);

  buttonInterruptOn();
  // interrupt lijkt foutmelding te veroorzaken, kan liggen aan combinatie met eeprom, wifi(manager)
  // volgens bronnen (https://circuits4you.com/2017/12/19/esp8266-fatal-exception-wdt-reset/)
  // kan het aan delay liggen dat ook in eeprom of wifimanager wordt gebruikt
}

void loop()
{

  if (buttonInterruptFlag == true)
  {
    buttonInterruptOff();
    decodeIP->loop(ip4, pressCount, maxShowTime, buttonPressTime);
    buttonInterruptFlag = false;
    resetIP = decodeIP->getRenewValue();
    buttonInterruptOn();
    if (resetIP == true)
    {
      Serial.println("reset should happen here");
      resetIP = false;
    }
  }
}
