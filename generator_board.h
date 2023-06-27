#ifndef __GENERATOR_BOARD_H__
#define __GENERATOR_BOARD_H__

#include <Arduino.h>
#include "gea_core.h"

/*
 * GEA addresses for the Arduino and the power boards
 */
#define LOCAL_ADDR 0x87

#define GEN1_ADDR 0x88
#define GEN2_ADDR 0x89
#define GEN3_ADDR 0x8a

extern int minPowerSteps;
extern int maxPowerSteps;

/*
 * Config request payload structure.
 */
typedef struct {
  uint8_t coil1_profile;
  uint8_t coil2_profile;
} __attribute__((__packed__))BoardConfigPayload_t;

/*
 * Power levels request payload structure.
 */
typedef struct {
  uint8_t coil1Power;
  uint8_t coil2Power;
  uint8_t heartbeat;
} __attribute__((__packed__))SetPowerLevelsPayload_t;

/*
 * Software version response payload structure.
 */
typedef struct {
  uint8_t crit_major;
  uint8_t crit_minor;
  uint8_t noncrit_major;
  uint8_t noncrit_minor;
} SoftwareVersion_t;

/*
 * Status response payload structure.
 */
typedef struct {
  uint16_t unk1;
  uint16_t unk2;
  uint16_t unk3;
  uint16_t unk4;
  uint16_t unk5;
  uint16_t halfBridge0_temp;
  uint16_t coil0_temp;
  uint16_t halfBridge1_temp;
  uint16_t coil1_temp;
  uint16_t acLineVoltage;
} Status_t;

/*
 * Command codes for the generator boards.
 */
typedef enum {
  CMD_GET_SW_VERSION=0x01,
  CMD_SET_BOARD_CONFIG=0x26,
  CMD_SET_PWR_LEVELS=0x28,
  CMD_GET_STATUS=0x9e
} GeaCommandList;

/*
 * Query response payload lengths.
 */
typedef enum {
  RESP_LENGTH_SW_VERSION=0x04,
  RESP_LENGTH_BOARD_CONFIG=0x00,
  RESP_LENGTH_PWR_LEVELS=0x02,
  RESP_LENGTH_STATUS=0x14
} GeaQueryRespPayloadLengthList;

/*
 * Coil power profiles.
 */
typedef enum {
  COIL_TYPE_NONE=0x00,
  COIL_TYPE_1800_WATT=0x01,
  COIL_TYPE_2500_WATT=0x02,
  COIL_TYPE_3200_WATT=0x03,
  COIL_TYPE_3700_WATT=0x04
} CoilProfileId;

int initSingleGenerator(uint8_t address, uint8_t profile1, uint8_t profile2);
int setPowerLevels(uint8_t address, uint8_t coil1Level, uint8_t coil2Level, uint8_t heartbeat);
char* getSoftwareVersion(uint8_t address);
Status_t getStatus(uint8_t address);
void printSoftwareVersions(int personality);
void printStatus(uint8_t address);

#endif
