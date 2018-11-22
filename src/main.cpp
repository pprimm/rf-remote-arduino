#include <Arduino.h>
#include <UIPEthernet.h>

#define INPUT_PIN 3
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

EthernetUDP udp;
IPAddress broadcastAddress(255,255,255,255);
const unsigned int UDP_PORT = 41794;
uint8_t mac[6] = {0x0A,0x08,0x02,0x03,0x04,0x05};

/**
 * Shared between ISR and loop()
 */
volatile unsigned char state = IDLE;
volatile unsigned char isrBytes[BYTE_COUNT];
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
volatile unsigned char byteIndex = 0;
void inputChange()
{
  input = digitalRead(INPUT_PIN);
  bitLevel = input ? 0x01 : 0x00;
  newTime = micros();
  tDiff = newTime - oldTime;
  oldTime = newTime;
  if ((tDiff < DT_T1 && input == LOW) || byteIndex >= BYTE_COUNT) {
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

  Ethernet.begin(mac);//,IPAddress(192,168,0,6));
  Serial.println(Ethernet.localIP());

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

void printBytes(unsigned char const* bytes, int byteCount)
{
  for (int i = 0;i < byteCount;++i) {
    printByte(bytes[i]);
    Serial.print(" ");
  }
  Serial.println();
}

unsigned char showBytes[BYTE_COUNT];
unsigned char showBitCount;
void loop() 
{
  if (state == TEMP) {
    showBitCount = bitCount;
    memcpy(showBytes,(const void*)isrBytes,sizeof(isrBytes));
    memset((void*)(isrBytes),0,sizeof(isrBytes));
    state = IDLE;
  }

  if (showBitCount == 77) {
    showBitCount = 0;
    printBytes(showBytes,sizeof(showBytes));
    if (udp.beginPacket(broadcastAddress,UDP_PORT)) {
      udp.write((const char*)(showBytes),sizeof(showBytes));
      udp.endPacket();
    }
  }
}