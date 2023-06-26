#include "input.h"
#include "config.h"
#include "utils.h"

#define MIN_ADC_RAWVALUE 0
#define MAX_ADC_RAWVALUE (2 ^ ADC_RESOLUTION) - 1

/*
 * @brief Uses a running average algorithm to read and filter ADC values. An array to pass filtered data should be provided as an argument.
 */
void readPotsAverage(uint16_t potValuesFiltered[]) {
  static uint16_t samples[numPots][numSamples];
  static uint8_t currentSample = 0;

  for (uint8_t i = 0; i < numPots; i++) {
    uint16_t rawAdcValue = analogRead(potPins[i]);
    
    samples[i][currentSample] = rawAdcValue;
    
    uint32_t sum = 0;

    for (uint8_t j = 0; j < numSamples; j++) {
      sum += samples[i][j];
    }

    potValuesFiltered[i] = sum / numSamples;
  }
}

void readPotsMapped(uint8_t potValuesMapped[], int min, int max) {
  uint16_t potValuesRaw[numPots];
  readPotsAverage(potValuesRaw);
  for (int i=0; i<numPots; i++) {
    potValuesMapped[i] = map(potValuesRaw[i], 0, 1023, min, max);
  }
}
