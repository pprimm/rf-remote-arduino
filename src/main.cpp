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
#define BYTE_COUNT 10


/**
 * Shared between ISR and loop()
 */
volatile unsigned char state = IDLE;
volatile unsigned char isrBytes[BYTE_COUNT];
volatile unsigned char byteIndex = 0;
volatile int bitCount = 0;
/*******************************************************************************
 * ISR for input change event
*******************************************************************************/
volatile unsigned char input;
volatile unsigned char bitLevel;
static unsigned long oldTime = 0;
static unsigned long newTime;
static unsigned long tDiff;
volatile unsigned char bitIndex = 0;
void inputChange()
{
  input = digitalRead(INPUT_PIN);
  bitLevel = input == LOW ? 0x01 : 0x00;
  newTime = micros();
  tDiff = newTime - oldTime;
  oldTime = newTime;
  if (tDiff < DT_T1 && input == LOW) {
    state = TEMP;
  }
  switch (state)
  {
    case IDLE:
      if (tDiff > DT_T3 && input == HIGH) {
        state = START;
        bitCount = 0;
        byteIndex = 0;
      }
      break;
    case START:
      if (tDiff < DT_T2) {
        state = END;
        break;
      }
      ++bitCount;
      byteIndex = bitCount / 8;
      bitIndex = bitCount % 8;
      isrBytes[byteIndex] |= bitLevel << bitIndex;
      break;
    case END:
      ++bitCount;
      state = START;
      byteIndex = bitCount / 8;
      bitIndex = bitCount % 8;
      isrBytes[byteIndex] |= bitLevel << bitIndex;
      break;
    case TEMP:
      break;
  }
}

void setup()
{
  Serial.begin(115200);
  Serial.println("Running");
  pinMode(INPUT_PIN, INPUT);
  attachInterrupt(digitalPinToInterrupt(INPUT_PIN), inputChange, CHANGE);
}

void printButton()
{
  Serial.print(bitCount,DEC);
  Serial.print(" ");
  Serial.print(byteIndex,DEC);
  Serial.print(" ");
  for (int i = 0;i <= byteIndex && i < BYTE_COUNT;++i) {
    Serial.print(isrBytes[i],HEX);
    Serial.print(" ");
  }
  Serial.println();
}

void loop() 
{
  if (state == TEMP) {
    if (bitCount == 77) {
      printButton();
    }
    memset((void*)(isrBytes),0,sizeof(isrBytes));
    state = IDLE;
  }
  delay(1);
}