#ifndef __CONFIG_H__
#define __CONFIG_H__

#include <Arduino.h>

#define __DEBUG__

/*
 * GPIO pin configuration. Change these to match your board!
 */

#define heartbeatLed LED_BUILTIN //        Toggles every time the heartbeat increments
#define pot1Pin PA0 //            Rear left burner control knob
#define pot2Pin PA1 //            Front left burner control knob
#define pot3Pin PA4 //            Center burner control knob
#define pot4Pin PB0 //            Front right burner control knob
#define pot5Pin PC1 //            Rear right burner control knob
#define personalitySelPin PB8 //  Personality select (0: 4 burners, 1: 5 burners)
#define dlbRelayCtrlPin PC7 //    Double line break relay control output
#define fanLowPin PB6 //          Low speed cooling fan output
#define fanHighPin PA7 //         High speed cooling fan output

#define geaUartRxPin PA10 // Receive pin for GEA bus
#define geaUartTxPin PA9 // Transmit pin for GEA bus

// Analog potentiometer pins
#define numPots 5
const int potPins[numPots] = {pot1Pin, pot2Pin, pot3Pin, pot4Pin, pot5Pin};

// The number of samples that are read for smoothing potentiometer data
const int numSamples = 10;

#endif
