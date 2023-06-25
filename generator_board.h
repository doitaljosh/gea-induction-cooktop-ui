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

typedef struct {
  GeaMessageHeader_t header;
  uint8_t coil1_profile;
  uint8_t coil2_profile;
} __attribute__((__packed__))BoardConfigMsg_t;

typedef struct {
  GeaMessageHeader_t header;
  uint8_t coil1Power;
  uint8_t coil2Power;
  uint8_t heartbeat;
} __attribute__((__packed__))SetPowerLevelsMsg_t;

/*
 * Command codes for the generator boards
 */
typedef enum {
  CMD_GET_SW_VERSION=0x01,
  CMD_SET_BOARD_CONFIG=0x26,
  CMD_SET_PWR_LEVELS=0x28,
  CMD_GET_CURRENTS=0x29,
  CMD_GET_STATUS=0x9e
} GeaCommandList;

/*
 * Coil power profiles
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

#endif
