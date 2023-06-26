#ifndef __INPUT_H__
#define __INPUT_H__

#include <Arduino.h>

void readPotsAverage(uint16_t potValuesFiltered[]);
void readPotsMapped(uint8_t potValuesMapped[], int min, int max);

#endif
