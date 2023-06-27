#include "generator_board.h"
#include "gea_core.h"
#include "crc16.h"
#include "utils.h"

/* 
 * The GE induction generator boards support 20 power levels. 
 * They will be mapped to raw ADC values from the pots. 
 */
int minPowerSteps = 0;
int maxPowerSteps = 19;
char consoleBuffer[256];

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
    sprintf(consoleBuffer, "E: Failed to transmit config message to address 0x%02X\n", address);
    Serial.print(consoleBuffer);
    return -1;
  }
}

/*
 * @brief Update the generator boards with the given power levels and heartbeat
 */
int setPowerLevels(uint8_t address, uint8_t coil1Level, uint8_t coil2Level, uint8_t heartbeat) {
  GeaCommandList cmd;
  SetPowerLevelsPayload_t payload;
  
  sprintf(consoleBuffer, "I: Addr: 0x%02X Coil1: %d Coil2: %d\n", address, coil1Level, coil2Level);
  Serial.print(consoleBuffer);

  if (!withinRange(coil1Level, minPowerSteps, maxPowerSteps) || !withinRange(coil2Level, minPowerSteps, maxPowerSteps)) {
    sprintf(consoleBuffer, "E: Zone values out of range for address 0x%02X\n", address);
    Serial.print(consoleBuffer);
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
    sprintf(consoleBuffer, "E: Failed to transmit zone control message to address 0x%02X\n", address);
    Serial.print(consoleBuffer);
    return -1;
  }
}

char* getSoftwareVersion(uint8_t address) {
  GeaTransmitMessage(address, CMD_GET_SW_VERSION, 0, 0);
  char* softwareVersionPayload = GeaReceivePayload(address, CMD_GET_SW_VERSION);
  if (sizeof(softwareVersionPayload) == 4) {
    return softwareVersionPayload;
  } else {
    sprintf(consoleBuffer, "E: Bad payload length: %d in software version message. Should be $d.", sizeof(softwareVersionPayload), 4);
    Serial.println(consoleBuffer);
    return NULL;
  }
}

Status_t getStatus(uint8_t address) {
  Status_t status;
  int responsePayloadLength = RESP_LENGTH_STATUS;
  char statusPayloadRequest[responsePayloadLength];

  for (int i=0; i<responsePayloadLength; i++) {
    statusPayloadRequest[i] = 0x00;
  }
  
  GeaTransmitMessage(address, CMD_GET_STATUS, statusPayloadRequest, responsePayloadLength);
  char* statusPayloadResponse = GeaReceivePayload(address, CMD_GET_STATUS);
  
  if (sizeof(statusPayloadResponse) == 20) {
    status.unk1 = statusPayloadResponse[0] << 8 | statusPayloadResponse[1];
    status.unk2 = statusPayloadResponse[2] << 8 | statusPayloadResponse[3];
    status.unk3 = statusPayloadResponse[4] << 8 | statusPayloadResponse[5];
    status.unk4 = statusPayloadResponse[6] << 8 | statusPayloadResponse[7];
    status.unk5 = statusPayloadResponse[8] << 8 | statusPayloadResponse[9];
    status.halfBridge0_temp = statusPayloadResponse[10] << 8 | statusPayloadResponse[11];
    status.coil0_temp = statusPayloadResponse[12] << 8 | statusPayloadResponse[13];
    status.halfBridge1_temp = statusPayloadResponse[14] << 8 | statusPayloadResponse[15];
    status.coil1_temp = statusPayloadResponse[16] << 8 | statusPayloadResponse[17];
    status.acLineVoltage = statusPayloadResponse[18] << 8 | statusPayloadResponse[19];

    return status;
  } else {
    sprintf(consoleBuffer, "E: Bad payload length: %d in status message. Should be %d.", sizeof(statusPayloadResponse), responsePayloadLength);
    Serial.println(consoleBuffer);
    return status;
  }
}

void printStatus(uint8_t address) {
  Status_t status = getStatus(address);

  uint16_t coil0Temp = status.coil0_temp;
  uint16_t halfBridge0Temp = status.halfBridge0_temp;
  uint16_t coil1Temp = status.coil1_temp;
  uint16_t halfBridge1Temp = status.halfBridge1_temp;
  uint16_t acLineVoltage = status.acLineVoltage;

  sprintf(
          consoleBuffer, 
          "I: Status for 0x%02X: Coil 0 Temp: %d*F, Coil 1 Temp: %d*F, H-Bridge 0 Temp: %d*F, H-Bridge 1 Temp: %d*F, AC Line Voltage: %dV",
          address, 
          coil0Temp,
          coil1Temp,
          halfBridge0Temp,
          halfBridge1Temp,
          acLineVoltage
          );
  Serial.println(consoleBuffer);
}
