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
#define BYTE_COUNT 12


/**
 * Shared between ISR and loop()
 */
volatile unsigned char state = IDLE;
volatile unsigned char isrBytes[BYTE_COUNT];
volatile unsigned char byteIndex = 0;
volatile unsigned char bitCount = 0;
/*******************************************************************************
 * ISR for input change event
*******************************************************************************/
// add byte msb first
// isrBytes[byteIndex] |= bitLevel << (7 - bitIndex);
// add byte lsb first
// isrBytes[byteIndex] |= bitLevel << bitIndex;

#define ADD_BIT(va,vb,vc,vd,ve) \
  vb = va / 8;                  \
  vc = va % 8;                  \
  vd[vb] |= ve << (7-vc);       \
  ++va;

volatile unsigned char input;
volatile unsigned char bitLevel;
volatile unsigned long oldTime;
volatile unsigned long newTime;
volatile unsigned long tDiff;
volatile unsigned char bitIndex;
void inputChange()
{
  input = digitalRead(INPUT_PIN);
  bitLevel = input ? 0x01 : 0x00;
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
        state = END;
        bitCount = 0;
        byteIndex = 0;
      }
      break;
    case START:
      if (tDiff > DT_T2) {
        state = TEMP;
        break;
      }
      ADD_BIT(bitCount,byteIndex,bitIndex,isrBytes,bitLevel)
      state = END;
      break;
    case END:
      if (tDiff < DT_T2) {
        state = START;
        break;
      }
      ADD_BIT(bitCount,byteIndex,bitIndex,isrBytes,bitLevel)
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
  oldTime = micros();
}

void printByte(unsigned char bytePrint) {
  //for (unsigned char bitPos = 0x80; bitPos; bitPos >>= 1) {
  //  Serial.write(bytePrint & bitPos ? '1' : '0');
  //}
  Serial.print(bytePrint >> 4,HEX);
  Serial.print(bytePrint & 0x0F,HEX);
  //Serial.print(bytePrint);
}

void printButton()
{
  for (int i = 0;i <= byteIndex && i < BYTE_COUNT;++i) {
    printByte(isrBytes[i]);
    Serial.print(" ");
  }
  Serial.println();
}

void loop() 
{
  if (state == TEMP) {
    //Serial.print(bitCount);
    //Serial.print(" ");
    if (bitCount == 77) {
      printButton();
    }
    memset((void*)(isrBytes),0,sizeof(isrBytes));
    state = IDLE;
  }
  delay(1);
}