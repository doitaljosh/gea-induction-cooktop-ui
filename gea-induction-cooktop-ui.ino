/*
 * Arduino-based Controller Firmware for GE Induction Cooktops
 * 
 * Design tips:
 * - Use pull-down resistors and decoupling capacitors on the analog pot inputs to prevent erratic behavior.
 * - Tie the personality pin high if you are controlling a 36 inch cooktop with 5 coils and 3 generator boards.
 * - To connect to the single-wire half-duplex serial bus the generator boards use, connect a bus transceiver. Refer to the following for design tips: https://github.com/wfang2002/Full-Half-Duplex-Adapter
 * 
 * Note: you MUST use an Arduino-compatible board that supports at least two hardware serial busses, or change Serial or Serial2 to use SoftwareSerial.
 * 
 * Written by Joshua Currier (doitaljosh)
 * 05/01/2022
 */

#include "config.h"
#include "utils.h"
#include "crc16.h"
#include "gea_core.h"
#include "generator_board.h"

int numberOfCoils;
int potValues[5];
int heartbeat;

HardwareSerial Serial1(PA10, PA9);

/*
 * Personality IDs
 */
typedef enum {
  PERSONALITY_FOUR_COILS=0,
  PERSONALITY_FIVE_COILS=1
} PersonalityId;

/*
 * @brief Configures and initializes all generator boards depending on the personality
 */
int initCooktop(int personality) {
  PersonalityId personalityName = (PersonalityId)personality;

  digitalWrite(dlbRelayCtrlPin, HIGH); // Turn on power to the generator boards
  digitalWrite(fanLowPin, HIGH); // Turn on the cooling fan

  delay(1000); // Allow generator firmware time to boot
  
  switch(personalityName) {
    case PERSONALITY_FOUR_COILS:
      initSingleGenerator(GEN1_ADDR, COIL_TYPE_2500_WATT, COIL_TYPE_2500_WATT);
      delay(100);
      initSingleGenerator(GEN2_ADDR, COIL_TYPE_3700_WATT, COIL_TYPE_1800_WATT);
      delay(100);
      setPowerLevels(GEN1_ADDR, 0, 0, 0);
      delay(100);
      setPowerLevels(GEN2_ADDR, 0, 0, 0);
      delay(100);
      break;
    case PERSONALITY_FIVE_COILS:
      initSingleGenerator(GEN1_ADDR, COIL_TYPE_2500_WATT, COIL_TYPE_2500_WATT);
      delay(100);
      initSingleGenerator(GEN2_ADDR, COIL_TYPE_3700_WATT, COIL_TYPE_NONE);
      delay(100);
      initSingleGenerator(GEN3_ADDR, COIL_TYPE_1800_WATT, COIL_TYPE_3200_WATT);
      delay(100);
      setPowerLevels(GEN1_ADDR, 0, 0, 0);
      delay(100);
      setPowerLevels(GEN2_ADDR, 0, 0, 0);
      delay(100);
      setPowerLevels(GEN3_ADDR, 0, 0, 0);
      delay(100);
      break;
    default:
      break;
  }

  return 0;
}

/*
 * @brief Setup hardware interfaces and call init functions
 */
void setup() {
  Serial.begin(115200); // Console logging
  Serial1.begin(19200);
  Serial.println("Initializing potentiometers...");

  for (int i=0; i<5; i++) {
    pinMode(potPins[i], INPUT);
  }
  pinMode(personalitySelPin, INPUT);
  pinMode(heartbeatLed, OUTPUT);
  pinMode(dlbRelayCtrlPin, OUTPUT);
  pinMode(fanLowPin, OUTPUT);
  pinMode(fanHighPin, OUTPUT);

  int personality;

  if (digitalRead(personalitySelPin) == 1) {
    personality = 1;
    numberOfCoils = 5;
  } else {
    personality = 0;
    numberOfCoils = 4;
  }

  
  Serial.print("Initializing generator boards for a ");
  if (personality == 1) {
    //Serial.println("36 inch cooktop...");
  } else {
    Serial.println("30 inch cooktop...");
  }
  

  initCooktop(personality);
  Serial.println("Starting main loop...");
}

/*
 * Cyclically process pot values and set power levels, incrementing the heartbeat every time the levels are updated.
 */
void loop() {
  for(heartbeat = 0; heartbeat < 256; heartbeat++) {
    digitalWrite(heartbeatLed, !digitalRead(heartbeatLed));

    
    Serial.print("Power levels: ");

    for(int i=0; i<5; i++) {
      potValues[i] = map(analogRead(potPins[i]), 41, 4095, minPowerSteps, maxPowerSteps);
      Serial.print(potValues[i]);
      Serial.print(" ");
    }

    Serial.println();
    
    switch(numberOfCoils) {
      case 4:
        setPowerLevels(GEN1_ADDR, potValues[0], potValues[1], heartbeat);
        setPowerLevels(GEN2_ADDR, potValues[2], potValues[3], heartbeat);
        break;
      case 5:
        setPowerLevels(GEN1_ADDR, potValues[0], potValues[1], heartbeat);
        setPowerLevels(GEN2_ADDR, potValues[4], 0, heartbeat);
        setPowerLevels(GEN3_ADDR, potValues[2], potValues[3], heartbeat);
        break;
    }

    delay(500);
  }
}