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
  BoardConfigPayload_t payload;

  payload.coil1_profile = profile1;
  payload.coil2_profile = profile2;

  int payloadSize = sizeof(BoardConfigPayload_t);

  char payloadBuf[payloadSize];
  memcpy(payloadBuf, &payload, payloadSize);

  if (GeaTransmitMessage(address, CMD_SET_BOARD_CONFIG, payloadBuf, payloadSize) == 0) {
    return 0;
  } else {
    Serial.print("Failed to transmit config message to address ");
    Serial.println(address);
    return -1;
  }
}

/*
 * @brief Update the generator boards with the given power levels and heartbeat
 */
int setPowerLevels(uint8_t address, uint8_t coil1Level, uint8_t coil2Level, uint8_t heartbeat) {
  GeaCommandList cmd;
  SetPowerLevelsPayload_t payload;

  if (!withinRange(coil1Level, minPowerSteps, maxPowerSteps) && !withinRange(coil2Level, minPowerSteps, maxPowerSteps)) {
    return -1;
  }
  
  payload.coil1Power = coil1Level;
  payload.coil2Power = coil2Level;
  payload.heartbeat = heartbeat;

  int payloadSize = sizeof(SetPowerLevelsPayload_t);
  
  char payloadBuf[payloadSize];
  memcpy(payloadBuf, &payload, payloadSize);
  
  if (GeaTransmitMessage(address, CMD_SET_PWR_LEVELS, payloadBuf, payloadSize) == 0) {
    return 0;
  } else {
    Serial.print("Failed to transmit control message to address ");
    Serial.println(address);
    return -1;
  }
}
