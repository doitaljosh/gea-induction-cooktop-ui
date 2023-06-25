#ifndef __CONFIG_H__
#define __CONFIG_H__

#include <Arduino.h>

/*
 * GPIO pin configuration. Change these to match your board!
 */

int heartbeatLed = LED_BUILTIN; //        Toggles every time the heartbeat increments

int pot1Pin = PA0; //            Rear left burner control knob
int pot2Pin = PA1; //            Front left burner control knob
int pot3Pin = PA4; //            Center burner control knob
int pot4Pin = PB0; //            Front right burner control knob
int pot5Pin = PC1; //            Rear right burner control knob
int personalitySelPin = PB8; //  Personality select (0: 4 burners, 1: 5 burners)

int dlbRelayCtrlPin = PC7; //    Double line break relay control output
int fanLowPin = PB6; //          Low speed cooling fan output
int fanHighPin = PA7; //         High speed cooling fan output

int potPins[5] = {pot1Pin, pot2Pin, pot3Pin, pot4Pin, pot5Pin};

#endif
