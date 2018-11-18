#include <Arduino.h>

#define RESET 0
#define ARM 1
#define CAPTURE 2

volatile unsigned long count  = 0;
volatile unsigned long newTime;
volatile unsigned long oldTime;
volatile unsigned long diff = 0;
volatile unsigned short state = RESET;

/**
 * 
  if (!arm) return;
  if (once) return;
  once = true;
  ++count;
  newTime = micros();
  diff = newTime - oldTime;
  oldTime = newTime;
  if (diff < min) {
    min = diff;
  }
 * 
**/

void counter()
{
  newTime = micros();
  switch (state)
  {
    case ARM:
      oldTime = micros();
      state = CAPTURE;
      break;
    case CAPTURE:
      diff = newTime - oldTime;
      state = RESET;
      break;
  }
}

void setup()
{
  Serial.begin(9600);
  Serial.println("Running");
  attachInterrupt(digitalPinToInterrupt(2), counter, CHANGE);
}

void loop() 
{
  Serial.print(count,DEC);
  Serial.print("  ");
  Serial.println(diff,DEC);
  state = ARM;
  diff = 0;
  delay(1000);
}