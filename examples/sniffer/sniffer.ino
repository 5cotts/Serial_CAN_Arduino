#include <Serial_CAN_Module.h>
#include <SoftwareSerial.h>

// INIT CLASSES
Serial_CAN CAN;

// SETTINGS
// tx of serial can module connect to D2
#define CAN_TX  2
// rx of serial can module connect to D3      
#define CAN_RX  3
// this seems to work        
#define SPEED 9600
// If the RANDOM_CAN define is set to 1, 
// this code is generating random CAN 
// packets in order to test the higher level code.
#define RANDOM_CAN 1

// INIT DATASTRUCTS
typedef struct {
  long id;
  byte rtr;
  byte ide;
  byte dlc;
  byte dataArray[20];
} packet_t;

// INIT GLOBALS
const char SEPARATOR = ',';
const char TERMINATOR = '\n';
const char RXBUF_LEN = 100;

// Printing a packet to serial
void printHex(long num) {
  if ( num < 0x10 ){ Serial.print("0"); }
  Serial.print(num, HEX);
}

void printPacket(packet_t * packet) {
  // packet format (hex string): [ID],[RTR],[IDE],[DATABYTES 0..8B]\n
  // example: 014A,00,00,1A002B003C004D\n
  printHex(packet->id);
  Serial.print(SEPARATOR);
  printHex(packet->rtr);
  Serial.print(SEPARATOR);
  printHex(packet->ide);
  Serial.print(SEPARATOR);
  // DLC is determinded by number of data bytes, format: [00]
  for (int i = 0; i < packet->dlc; i++) {
    printHex(packet->dataArray[i]);
  }
  Serial.print(TERMINATOR);
}

// CAN packet simulator
void CANsimulate(void) {
  packet_t txPacket;

  long sampleIdList[] = {0x110, 0x18DAF111, 0x23A, 0x257, 0x412F1A1, 0x601, 0x18EA0C11};
  int idIndex = random (sizeof(sampleIdList) / sizeof(sampleIdList[0]));
  int sampleData[] = {0xA, 0x1B, 0x2C, 0x3D, 0x4E, 0x5F, 0xA0, 0xB1};

  txPacket.id = sampleIdList[idIndex];
  txPacket.ide = txPacket.id > 0x7FF ? 1 : 0;
  txPacket.rtr = 0; //random(2);
  txPacket.dlc = random(1, 9);

  for (int i = 0; i < txPacket.dlc ; i++) {
    int changeByte = random(4);
    if (changeByte == 0) {
      sampleData[i] = random(256);
    }
    txPacket.dataArray[i] = sampleData[i];
  }

  printPacket(&txPacket);
}

void setup()
{
    Serial.begin(SPEED);
    #if RANDOM_CAN
        randomSeed(12345);
    #else
        CAN.begin(CAN_TX, CAN_RX, SPEED);      // tx, rx
    #endif
}

// send(unsigned long id, byte ext, byte rtrBit, byte len, const byte *buf);
void loop()
{
    #if RANDOM_CAN
        CANsimulate();
        delay(100);
    #else
        unsigned long id = 0;
        unsigned char dta[8];

        if(CAN.recv(&id, dta))
        {
            Serial.print("GET DATA FROM ID: ");
            Serial.println(id);
            for(int i=0; i<8; i++)
            {
                Serial.print("0x");
                Serial.print(dta[i], HEX);
                Serial.print('\t');
            }
            Serial.println();
        }
    #endif
}

// END FILE
