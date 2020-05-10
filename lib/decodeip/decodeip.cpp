#include "decodeip.h"

uint8_t DecodeIP::atoi8_t(String s) 
{
    uint8_t i;
    uint8_t n;
    n = 0;
    for (i = 0; s[i] >= '0' && s[i] <= '9'; i++)
        n = 10 * n +(s[i] - '0');
    return n;
}

void DecodeIP::flashResult(uint8_t block, uint8_t numberPosition)
{
    /* flash the pins, according to the timerCheck, the result- and the acknowledge array */
    /* result is a 4 dimensional array
       4 ip number blocks 
       3 representation of hundreds, tens and number-below-ten, power of ten system
       3 rows for representing a number, 1-9, divided by 3, we use 3 pins 
            a zero will be representend in the most left position of the first row
       3 columns for each result pin, we use 3 result pins

       acknowledge pin should have a value when the last result-row is given
    */
    Serial.print("start flashResult met block: ");
    Serial.print(block);
    Serial.print(" en IP-positie: ");
    Serial.println(numberPosition);
    if (block > 0) {
      block--;   // index starts at 0
    }
    if (block < 4)   // four blocks (0,1,2,3)
    {
      // show which block, first attention by flash them all

      // flash value
      //for (uint8_t powerTen = 2; powerTen >= 0; powerTen--)
      uint8_t powerTen = 3 - numberPosition;
      //{
      bool ready = false;
      uint8_t row = 0;
      while (! ready)
      {
          this->delayInMillis(250);
          for (uint8_t col = 0; col < 3; col++)
          {
              if (this->result[block][powerTen][row][col] == 1)
              {
                  digitalWrite(this->pin[col], HIGH);
                  if (this->acknowledge[block][powerTen][row] == 1)
                  {
                      digitalWrite(this->pin[3], HIGH);
                      ready = true; // soft break;
                  }
                  this->delayInMillis(250);
                  //digitalWrite(this->pin[col], LOW);
              }
              digitalWrite(this->pin[0], LOW);
              digitalWrite(this->pin[1], LOW);
              digitalWrite(this->pin[2], LOW);
              digitalWrite(this->pin[3], LOW);
              this->delayInMillis(250);
          }
          row++;
      }
    }
    Serial.println("end flashResult");
}

void DecodeIP::setResultZero()
{
    for (uint8_t block = 0; block < 4; block++)
    {
        for (uint8_t tenPower = 0; tenPower < 3; tenPower++)
        {
            for (uint8_t row = 0; row < 4; row++)
            {
                for (uint8_t col = 0; col < 3; col++)
                {
                    this->result[block][tenPower][row][col] = 0;
                }
                this->acknowledge[block][tenPower][row] = 0;
            }
        }
    }
}

void DecodeIP::setResult(uint8_t block, uint8_t value)
{
    uint8_t tenPower = 2;   // from ten-powered number system

    if (value > 99)
    {
        uint8_t hundreds = int8_t(value / 100);
        for (uint8_t pos = 0; pos < hundreds; pos++)
        {
            this->result[block][tenPower][0][pos] = 1;
            this->acknowledge[block][tenPower][0] = 1;
        }
        value -= hundreds * 100;
    }
    else
    {
        /* tens value is zero: set nost left value to one */ 
        this->result[block][tenPower][0][2] = 1;
        this->acknowledge[block][tenPower][0] = 1;
    }
    
    tenPower = 1;
    if (value > 9)
    {
        uint8_t tens = int8_t(value / 10);
        for (uint8_t pos = 0; pos < tens; pos++)
        {
            uint8_t col = pos % 3;
            uint8_t row = uint8_t(pos / 3);
            this->result[block][tenPower][row][col] = 1;
            if (tens - pos == 1)
            {
                this->acknowledge[block][tenPower][row] = 1;
            }
        }
        value -= tens * 10;
    }
    else
    {
        /* tens value is zero: set most left value to one */ 
        this->result[block][tenPower][0][2] = 1;
        this->acknowledge[block][tenPower][0] = 1;
    }

    // now for the least significant number
    tenPower = 0;
    if (value >  0)
    {
        for (uint8_t pos = 0; pos < value; pos++)
        {
            uint8_t col = pos % 3;
            uint8_t row = uint8_t(pos / 3);
            this->result[block][tenPower][row][col] = 1;
            if (value - pos == 1)
            {
                this->acknowledge[block][tenPower][row] = 1;
            }
        }
    }
    else
    {
        /* tens value is zero: set most left value to one */ 
        this->result[block][tenPower][0][2] = 1;
        this->acknowledge[block][tenPower][0] = 1;
    }
}

void DecodeIP::process(String ip4)
{
    this->setResultZero();  // initialise result/acknowledge arrays

    String block = "";
    uint8_t blockCount = 0;
    bool stop = false;
    for (uint8_t i = 0; i <= ip4.length(); i++) {
        if ((ip4[i] == '.') || (i == ip4.length())) {
            stop = true;
        }
        else {
            block += ip4[i];
        }
        if (stop == true)
        {
            uint8_t value = this->atoi8_t(block);  // max 256
            Serial.println(value);
            stop = false;
            block = "";
            this->setResult(blockCount, value);
            blockCount += 1;
        }
    }
}

void DecodeIP::loop(String ip4, uint32_t buttonPressTime)
{
  this->buttonPressTime = buttonPressTime;

  if (this->ip4 != ip4) {
    // refreshed ip
    this->ip4 = ip4;
    this->pressCount = 0;
    this->process(this->ip4);
  }
  if (digitalRead(this->pinButton) == HIGH)
  {
    this->actOnHigh();
  }
  else
  {
    this->actOnLow();
  }
}

void DecodeIP::actOnHigh()
{
  uint8_t block = int(this->buttonPressTime / 1000);
  this->allowShowResult = true;
  if (block > 0)
  {
    this->savedBlock = block;
    this->allowShowResult = false;
  }
  //Serial.print("savedBlock: ");
  //Serial.println(this->savedBlock);
  //Serial.print("pressCount: ");
  //Serial.println(this->pressCount);
  if (this->savedBlock == 1)
  {
    digitalWrite(this->pin[3], HIGH);
    this->pressCount = 0; 
    this->maxShowTime = 0;
  }
  if (this->savedBlock == 2)
  {
    digitalWrite(this->pin[0], HIGH);
  }
  if (this->savedBlock == 3)
  {
    digitalWrite(this->pin[1], HIGH);
  }
  if (this->savedBlock == 4)
  {
    digitalWrite(this->pin[2], HIGH);
  }
  if (this->savedBlock == 5)
  {
    digitalWrite(this->pin[1], LOW);
    digitalWrite(this->pin[2], LOW);
  }
  if (this->savedBlock > 6)
  {
      this->savedBlock = 6;
  }
  if (this->savedBlock == 6)
  {
    for (uint8_t i = 0; i < this->savedBlock; i++)
    {
      //this->flashPin(this->pin[2], 100);
      //this->flashPin(this->pin[1], 100);
      //this->flashPin(this->pin[0], 100);
      //this->flashPin(this->pin[3], 100);
    }
  }
}

void DecodeIP::actOnLow()
{
  if ((this->savedBlock >= 1) && (this->savedBlock <= 4))
  {
    if (this->pressCount == 0)
    {
      this->flashBlock(this->savedBlock);  // show which block is being read
    }
  }
  if (this->savedBlock == 0)
  {
    this->pressCount++;
  }
  if (this->allowShowResult == true)
  {
    if (this->maxShowTime < this->MAXSHOW)
    {
      if (this->pressCount > 3)
      {
        this->pressCount = 1;
        for (uint8_t i = 0; i < 3; i++)
        {
          //this->flashPin(this->pin[3], 100);
          //this->flashPin(this->pin[0], 100);
          //this->flashPin(this->pin[1], 100);
          //this->flashPin(this->pin[2], 100);
        }
        this->maxShowTime = this->MAXSHOW;  // soft break
      }
      this->flashResult(this->savedBlock, this->pressCount);
      this->maxShowTime++;
    }
  }
  if (this->savedBlock == 6)
  {    
    // this->renewValue will be returned false after releasing the button
    this->renewValue = true;
  }
}

void DecodeIP::flashBlock(uint8_t block)
{
  unsigned long wait = 100;
  digitalWrite(this->pin[3], HIGH);
  if (block > 1)
  digitalWrite(this->pin[0], HIGH);
  if (block > 2)
  digitalWrite(this->pin[1], HIGH);
  if (block > 3)
  digitalWrite(this->pin[2], HIGH);

  this->delayInMillis(wait);

  digitalWrite(this->pin[3], LOW);
  digitalWrite(this->pin[0], LOW);
  digitalWrite(this->pin[1], LOW);
  digitalWrite(this->pin[2], LOW);

  this->delayInMillis(wait);
}



void DecodeIP::flashPin(uint8_t pin, uint8_t ms) {
  digitalWrite(pin, HIGH);
  for (uint16_t i = 0; i <= ms; i++)
  {
    delayMicroseconds(250);   // delay in the loop could cause an exception (9) when using interrupts
    delayMicroseconds(250);   // delay in the loop could cause an exception (9) when using interrupts
    delayMicroseconds(250);   // delay in the loop could cause an exception (9) when using interrupts
    delayMicroseconds(250);   // delay in the loop could cause an exception (9) when using interrupts
  }
  digitalWrite(pin, LOW);
}

void DecodeIP::delayInMillis(uint8_t ms) {
  for (uint16_t i = 0; i <= ms; i++)
  {
    delayMicroseconds(250);   // delay in the loop could cause an exception (9) when using interrupts
    delayMicroseconds(250);   // delay in the loop could cause an exception (9) when using interrupts
    delayMicroseconds(250);   // delay in the loop could cause an exception (9) when using interrupts
    delayMicroseconds(250);   // delay in the loop could cause an exception (9) when using interrupts
  }
}

bool DecodeIP::getRenewValue()
{
  return this->renewValue;
}