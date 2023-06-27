#include "crc16.h"

uint16_t Crc16ProcessByte(const uint16_t seed, const uint8_t byte) {
  return (uint16_t)(crc16Table[((seed >> 8) ^ byte) & 0x00FF] ^ (seed << 8));
}

/*
 * @brief Calculate a CRC16 given a data buffer and size
 */
uint16_t CalculateCrc16(char *dataToCheck, int sizeOfData) {
  uint16_t crc = 0xe300;
  int index;

  for (index = 0; index < sizeOfData; index++) {
    const uint8_t current = dataToCheck[index];
    crc = Crc16ProcessByte(crc, current);
  }

  return crc; 
}
