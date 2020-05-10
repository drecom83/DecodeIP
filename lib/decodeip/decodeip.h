#include <ESP8266WiFi.h>          //ESP8266 Core WiFi Library (you most likely already have this in your sketch)

#ifndef DECODE_IP_H    // To make sure you don't declare the function more than once by including the header multiple times.
#define DECODE_IP_H

/* let pins flash according to a given ip4 number
   using 1 acknowledge pin and 3 result pins
*/
class DecodeIP
{
private:
    /* paramer for showBlock */
    const uint8_t MAXSHOW = 10;
    uint8_t maxShowTime = MAXSHOW;

    /* count the number of button-presses within a choosen IP-block */
    uint8_t pressCount = 0;


    /* remember which block in turn */
    uint8_t savedBlock = 0;

    /* if a block is selected then allowShowResult = true */
    bool allowShowResult = false;

    /* 
    pin[0], pin[1], pin[2] are result pins
    pin[3] is acknowledge pin
    */
    uint8_t pin[4];

    /* button to take action on */
    uint8_t pinButton;

    /* time elapsed since buttonPress */
    uint32_t buttonPressTime;

    /* after 6 seconds a renew is true */
    bool renewValue = false; 

    /* convert string to number */
    uint8_t atoi8_t(String s);
    
    String ip4 = "0.0.0.0";

    /* result is a 4 dimensianal array
       4 ip number blocks 
       3 representation of hundreds, tens and number-below-ten, power of ten system
       3 rows for representing a number, 1-9, divided by 3, we use 3 pins 
            a zero will be representend in the most left position of the first row
       3 columns for each result pin, we use 3 result pins
    */
    uint8_t result[4][3][3][3];

    /* acknowledge pin should have a value when the last result-row is given */
    uint8_t acknowledge[4][3][3];

    /* set all values of result and acknowledge to zero */
    void setResultZero();

    /* set values in 3-dimensialnal arry result */
    void setResult(uint8_t block, uint8_t value);
   
    /* does the response on the acknowledge/result pins */
    void flashResult(uint8_t block, uint8_t numberPosition);

    /* do actions needed when pinButton == HIGH */
    void actOnHigh();

    /* do actions needed when pinButton == LOW */
    void actOnLow();

    void delayInMillis(uint8_t ms);
    void flashPin(uint8_t pin, uint8_t ms);

public:
    DecodeIP()
    {
    }

    DecodeIP(uint8_t acknowledgePin, uint8_t resultPin1, uint8_t resultPin2, uint8_t resultPin3, uint8_t pinButton)
    {
        this->pin[0] = resultPin1;
        this->pin[1] = resultPin2;
        this->pin[2] = resultPin3;
        this->pin[3] = acknowledgePin;
        this->pinButton = pinButton;
    };

    ~DecodeIP()
    {
    };

    /* processes the IP4 string for use on the result/acknowledge pins */ 
    void process(String ip4);

    /* loop of this class */
    /* if true the device should reset to Access Point with default IP address */
    void loop(String ip4, uint32_t buttonPressTime);

    /* number of pins flashing/on to show which block is used */
    void flashBlock(uint8_t block);

     /* give renewValue */
    bool getRenewValue();
};
#endif