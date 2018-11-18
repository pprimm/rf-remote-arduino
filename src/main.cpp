#include <Arduino.h>

volatile uint32_t count  = 0;

void counter()
{
  ++count;
}

void setup()
{
  Serial.begin(9600);
  Serial.println("Running");
  attachInterrupt(digitalPinToInterrupt(2), counter, CHANGE);
}

void loop() 
{
  Serial.println(count,DEC);
  delay(1000);
}