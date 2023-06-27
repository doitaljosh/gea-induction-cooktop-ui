#include <Arduino.h>
#include "gea_core.h"
#include "crc16.h"
#include "utils.h"
#include "config.h"

int escapedBytes = 0;

/*
 * @brief Check if a byte needs to be escaped.
 */
bool isEscaped(uint8_t value) {
  switch(value) {
    case GEA_ESC:
    case GEA_ACK:
    case GEA_SOF:
    case GEA_EOF:
      return true;
    default:
      return false;
  }
}

/*
 * @brief Returns an escaped version of an unescaped GEA message buffer
 */
char* escapeMessage(char* unescapedMsg, size_t length) {
  // Set the global variable to zero
  escapedBytes = 0;
    
  // Calculate the maximum possible size of the escaped message
  size_t escapedLength = length;
  for (size_t i = 1; i < length - 1; i++) {
      if (isEscaped(unescapedMsg[i])) {
          escapedLength++;
          // Set the global variable, so other functions know how many escaped bytes were found.
          escapedBytes = escapedLength - length;
      }
  }

  // Create a new escaped message buffer
  char* escapedMsg = (char*)malloc(escapedLength * sizeof(char));
  size_t j = 0;
  // Copy the first byte unchanged
  escapedMsg[j++] = unescapedMsg[0];

  // Escape the bytes in the middle of the array
  for (size_t i = 1; i < length - 1; i++) {
      if (isEscaped(unescapedMsg[i])) {
          escapedMsg[j++] = GEA_ESC;
      }
      escapedMsg[j++] = unescapedMsg[i];
  }

  // Copy the last byte unchanged
  escapedMsg[j] = unescapedMsg[length - 1];

  return escapedMsg;
}

/*
 * Returns an unescaped version of an escaped GEA message buffer.
 */
char* unescapeMessage(char* escapedMsg) {
  size_t escapedLength = sizeof(escapedMsg);
  char* unescapedMsg = (char*)malloc(escapedLength + 1);

  if (unescapedMsg == NULL) {
    Serial.println("E: Failed to allocate memory for unescaped RX buffer");
    return NULL;
  }

  int unescapedIdx = 0;
  int escapedIdx = 0;

  while (escapedIdx < escapedLength) {
    if (escapedMsg[escapedIdx] == GEA_ESC) {
      escapedIdx++;

      if (escapedIdx >= escapedLength) {
        Serial.println("E: escapedIdx is out of range. Invalid escape sequence.");
        free(unescapedMsg);
        return NULL;
      }

      char escapedByte = escapedMsg[escapedIdx];

      switch (escapedByte) {
        case GEA_ESC:
          unescapedMsg[unescapedIdx] = GEA_ESC;
          break;
        case GEA_ACK:
          unescapedMsg[unescapedIdx] = GEA_ACK;
          break;
        case GEA_SOF:
          unescapedMsg[unescapedIdx] = GEA_SOF;
          break;
        case GEA_EOF:
          unescapedMsg[unescapedIdx] = GEA_EOF;
          break;
        default:
          Serial.println("switch(escapedByte) defaulted. Invalid escape sequence.");
          free(unescapedMsg);
          return NULL;
      }
    } else {
      unescapedMsg[unescapedIdx] = escapedMsg[escapedIdx];
    }
    unescapedIdx++;
    escapedIdx++;
  }
  unescapedMsg[unescapedIdx] = '\0';
  return unescapedMsg;
}

/*
 * @brief Reads a GEA message from the serial bus if it is available, then stores it into a buffer for further processing.
 */
void GeaReceiveMessage(char* rxBufferEscaped, int rxBufferSize) {
  int idx = 0;
  int escapeFlag = 0;

  if (Serial1.available()) {
    char rxByte = Serial1.read();

    switch (rxByte) {
      case GEA_SOF:
        idx = 0;
        escapeFlag = 0;
        break;
      case GEA_EOF:
        rxBufferEscaped[idx] = '\0';
        idx = 0;
        break;
      case GEA_ESC:
        escapeFlag = 1;
        break;
      default:
        if (idx < rxBufferSize - 1) {
          if (escapeFlag) {
            rxByte ^= 0x20;
            escapeFlag = 0;
          }
          rxBufferEscaped[idx++] = rxByte;
        }
    }
  }
}

/*
 * Builds a GEA message frame given the destination, command, payload buffer, and payload length, then transmits it over the serial bus.
 */
int GeaTransmitMessage(byte dst, byte cmd, char* payload, int payloadLength) {
  int msgBufLength = payloadLength + GEA_OVERHEAD;
  char* message = (char*)malloc(msgBufLength);

  if (message == NULL) {
    Serial.println("E: Failed to allocate memory for TX buffer");
    return NULL;
  }

  message[0] = GEA_SOF;
  message[1] = dst;
  message[2] = msgBufLength;
  message[3] = LOCAL_ADDR;
  message[4] = cmd;

  // Copy payload data
  int i;
  for (i=0; i<payloadLength; i++) {
    message[5+i] = payload[i];
  }

  // Calculate CRC16
  uint16_t crc16 = CalculateCrc16(message, payloadLength + 5);
  message[5+payloadLength] = crc16 >> 8;
  message[5+payloadLength+1] = crc16 & 0xFF;

  // Append EOF byte
  message[5+payloadLength+2] = GEA_EOF;

  // Escape the message and transmit it
  char* escapedMessage = escapeMessage(message, msgBufLength);
#ifdef __DEBUG__
  Serial.print("D: GEA TX: ");
  printHexByteArray(escapedMessage, msgBufLength + escapedBytes);
#endif
  Serial1.write(escapedMessage, msgBufLength + escapedBytes);
  Serial1.write(GEA_ACK);

  free(message);

  return 0;
}

GeaMessage_t GeaValidateAndParseReceivedMessage(char* rxBuffer, size_t rxBufferSize) {
  GeaMessage_t msg;
  msg.payload = NULL;

  char* rxBufferUnescaped = unescapeMessage(rxBuffer);

  // Check the SOF
  if (rxBufferUnescaped[0] != GEA_SOF) {
    Serial.println("E: Invalid GEA message: Invalid STX received");
    return msg;
  }

  msg.destination = rxBufferUnescaped[1];
  msg.length = rxBufferUnescaped[2];
  msg.source = rxBufferUnescaped[3];
  msg.command = rxBufferUnescaped[4];

  // Validate the length
  size_t expectedLength = msg.length;
  if (rxBufferSize != expectedLength) {
    Serial.println("E: Invalid GEA message: Length mismatch.");
    return msg;
  }

  // Validate the CRC16
  uint16_t expectedCrc16 = (rxBufferUnescaped[rxBufferSize - 3] << 8) | rxBufferUnescaped[rxBufferSize - 2];
  uint16_t calculatedCrc16 = CalculateCrc16(rxBufferUnescaped, rxBufferSize - 3);
  if (expectedCrc16 != calculatedCrc16) {
    Serial.println("E: Invalid GEA message: Checksum mismatch.");
    return msg;
  }

  // Check the ETX
  if (rxBufferUnescaped[rxBufferSize - 1] != GEA_EOF) {
    Serial.println("E: Invalid GEA message: ETX not found or unexpected EOF.");
    return msg;
  }

  // Calculate the length of the payload.
  size_t payloadLength = msg.length - GEA_OVERHEAD;

  // Allocate memory for the payload struct member.
  msg.payload = (uint8_t*)realloc(msg.payload, payloadLength * sizeof(uint8_t));
  if (msg.payload == NULL) {
    Serial.println("E: Failed to allocate memory for GEA message payload.");
    return msg;
  }

  // Copy payload data from unescaped message buffer;
  memcpy(msg.payload, rxBufferUnescaped + 5, payloadLength);

  return msg;
}

void GeaUnallocatePayloadMemory(GeaMessage_t* msg) {
  free(msg->payload);
  msg->payload = NULL;
}

char* GeaReceivePayload(uint8_t sourceAddress, uint8_t command) {
  char rxBufferEscaped[GEA_RXBUF_SIZE];
  
  GeaReceiveMessage(rxBufferEscaped, GEA_RXBUF_SIZE);
  
  char* rxBufferUnescaped = unescapeMessage(rxBufferEscaped);

  uint8_t messageLength = rxBufferUnescaped[2];

  GeaMessage_t msg = GeaValidateAndParseReceivedMessage(rxBufferUnescaped, rxBufferUnescaped[2]);
  int payloadLength = msg.length - GEA_OVERHEAD;
  char payload[payloadLength];

  if (msg.source == sourceAddress && msg.command == command) {
    for (int i = 0; i < payloadLength; i++) {
      payload[i] = msg.payload[i];
    }

    GeaUnallocatePayloadMemory(&msg);
    return payload;
  }
}
