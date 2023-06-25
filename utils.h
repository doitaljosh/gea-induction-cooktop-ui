#ifndef __UTILS_H__
#define __UTILS_H__

#include <Arduino.h>

bool withinRange(int value, int minimum, int maximum);
void printHexByteArray(char* byteArray, size_t length);

#endif
