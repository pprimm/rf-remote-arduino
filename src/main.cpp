#include <Arduino.h>

#define INPUT_PIN 2
// ISR States
#define IDLE 0
#define START 1
#define END 2
#define TEMP 3
// Bit stream timings
#define DT_T1 300
#define DT_T2 600
#define DT_T3 1600
#define BYTE_COUNT 32


/**
 * Shared between ISR and loop()
 */
volatile unsigned char state = IDLE;
volatile unsigned char isrBytes[BYTE_COUNT];
volatile unsigned char byteIndex = 0;
volatile unsigned char bitIndex = 0;
volatile unsigned char input = LOW;
volatile int bitCount = 0;
volatile int loopCount = 0;
/*******************************************************************************
 * ISR for input change event
*******************************************************************************/
static unsigned long oldTime = 0;
static unsigned long newTime;
static unsigned long tDiff;
void inputChange()
{
  input = digitalRead(INPUT_PIN);
  newTime = micros();
  tDiff = newTime - oldTime;
  oldTime = newTime;
  ++loopCount;
  if (tDiff < DT_T1) {
    state = TEMP;
  }
  switch (state)
  {
    case IDLE:
      if (tDiff > DT_T3 && input == HIGH) {
        state = START;
        bitCount = 0;
      }
      break;
    case START:
      if (tDiff < DT_T2) {
        state = END;
        break;
      }
      ++bitCount;
      break;
    case END:
      ++bitCount;
      state = START;
      break;
    case TEMP:
      break;
  }
}

void setup()
{
  Serial.begin(9600);
  Serial.println("Running");
  pinMode(INPUT_PIN, INPUT);
  attachInterrupt(digitalPinToInterrupt(INPUT_PIN), inputChange, CHANGE);
}

void loop() 
{
  Serial.print(loopCount,DEC);
  Serial.print("   ");
  Serial.println(bitCount,DEC);
  state = IDLE;
  bitCount = 0;
  loopCount = 0;
  delay(2000);
}