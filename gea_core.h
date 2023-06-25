#ifndef __GEA_CORE_H__
#define __GEA_CORE_H__

#include <Arduino.h>

extern int escapedBytes;

#define GEA_OVERHEAD 0x08
#define LOCAL_ADDR 0x87;

typedef enum {
  GEA_ESC = 0xe0, // Escape
  GEA_ACK = 0xe1, // Acknowledge
  GEA_SOF = 0xe2, // Start of frame
  GEA_EOF = 0xe3 // End of frame
} GeaHeaderBytes;

/* 
 * Data structure of a basic GEA packet header
 */
typedef struct {
  uint8_t sof;
  uint8_t destination;
  uint8_t length;
  uint8_t source;
  uint8_t command;
} __attribute__((__packed__))GeaMessageHeader_t;

bool isEscaped(uint8_t value);
char* escapeMessage(char* unescapedMsg, size_t length);
char* unescapeMessage(char* escapedMsg);
void GeaReceiveMessage(char* rxBufferEscaped, int rxBufferSize);
int GeaTransmitMessage(byte dst, byte cmd, const byte* payload, int payloadLength);

#endif
