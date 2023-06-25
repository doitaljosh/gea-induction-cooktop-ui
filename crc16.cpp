#include "crc16.h"

/*
 * @brief Calculate a CRC16 given a data buffer and size
 */
uint16_t CalculateCrc16(char *dataToCheck, int sizeOfData) {
  uint16_t crc16;
  char *ptr;
  size_t a;

  crc16 = 0xe300;
  ptr = dataToCheck;

  if (ptr != NULL) {
    for(a=0; a<sizeOfData; a++) {
      crc16 = (crc16 >> 8) ^ crc16Table[(crc16 ^ (uint16_t)*ptr++) & 0x00ff];
    }
  }
  return crc16; 
}
