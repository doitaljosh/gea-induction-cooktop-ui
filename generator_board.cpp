#include "generator_board.h"
#include "gea_core.h"
#include "crc16.h"
#include "utils.h"

/* 
 * The GE induction generator boards support 20 power levels. 
 * These must be mapped to ADC values from the pots. 
 */
int minPowerSteps = 0;
int maxPowerSteps = 19;

/*
 * @brief Initialize a generator board at an address and tell it what type of coils are connected
 */
int initSingleGenerator(uint8_t address, uint8_t profile1, uint8_t profile2) {
  GeaCommandList cmd;
  BoardConfigMsg_t msg;

  msg.header.sof = GEA_SOF;
  msg.header.destination = address;
  msg.header.length = 0x0a;
  msg.header.source = LOCAL_ADDR;
  msg.header.command = CMD_SET_BOARD_CONFIG;
  msg.coil1_profile = profile1;
  msg.coil2_profile = profile2;

  int txBufferSize = (sizeof(BoardConfigMsg_t) + 3);
  char txBuffer[txBufferSize];

  memcpy(txBuffer, &msg, (txBufferSize - 3));

  uint16_t crc16 = CalculateCrc16(txBuffer, (txBufferSize - 3));

  txBuffer[txBufferSize - 3] = crc16 & 0xff;
  txBuffer[txBufferSize - 2] = crc16 >> 8;
  txBuffer[txBufferSize - 1] = GEA_EOF;

  char* txBufferAck = escapeMessage(txBuffer, msg.header.length);

  printHexByteArray(txBufferAck, sizeof(txBuffer) + escapedBytes);
  Serial1.write(txBufferAck, sizeof(txBuffer) + escapedBytes);

  free(txBuffer);
  free(txBufferAck);
    
  return 0;
}

/*
 * @brief Update the generator boards with the given power levels and heartbeat
 */
int setPowerLevels(uint8_t address, uint8_t coil1Level, uint8_t coil2Level, uint8_t heartbeat) {
  GeaCommandList cmd;
  SetPowerLevelsMsg_t msg;

  if (!withinRange(coil1Level, minPowerSteps, maxPowerSteps) && !withinRange(coil2Level, minPowerSteps, maxPowerSteps)) {
    return -1;
  }
  
  msg.header.sof = GEA_SOF;
  msg.header.destination = address;
  msg.header.length = 0x0b;
  msg.header.source = LOCAL_ADDR;
  msg.header.command = CMD_SET_PWR_LEVELS;
  msg.coil1Power = coil1Level;
  msg.coil2Power = coil2Level;
  msg.heartbeat = heartbeat;
  
  int txBufferSize = (sizeof(SetPowerLevelsMsg_t) + 3);
  char txBuffer[txBufferSize];
 
  memcpy(txBuffer, &msg, (txBufferSize - 3));
   
  uint16_t crc16 = CalculateCrc16(txBuffer, (txBufferSize - 3));
  txBuffer[txBufferSize - 3] = crc16 & 0xff;
  txBuffer[txBufferSize - 2] = crc16 >> 8;
  txBuffer[txBufferSize - 1] = GEA_EOF;
  
  char* txBufferAck = escapeMessage(txBuffer, msg.header.length);
  
  Serial.print("txBufferSize=");
  Serial.println(sizeof(txBufferAck));
  
  printHexByteArray(txBufferAck, sizeof(txBuffer) + escapedBytes);
  Serial1.write(txBufferAck, sizeof(txBuffer) + escapedBytes);
  
  free(txBuffer);
  free(txBufferAck);
    
  return 0;
}
