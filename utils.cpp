#include "utils.h"
#include <Arduino.h>

/*
 * @brief Check if a given value is between a defined minimum and maximum 
 */
bool withinRange(int value, int minimum, int maximum) {
  return (minimum <= value && value <= maximum);
}

/*
 * @brief Prints a series of hex characters to the serial console, given a byte array
 */
void printHexByteArray(char* byteArray, size_t length) {
  for (size_t i = 0; i < length; i++) {
    if (byteArray[i] < 0x10) {
      Serial.print("0");
    }
    Serial.print(byteArray[i], HEX);
    Serial.print(" ");
  }
  Serial.println();
}
