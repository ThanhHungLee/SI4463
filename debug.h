#ifndef __DEBUG_H
#define __DEBUG_H

#include "Userdefine.h"

typedef enum{
	DEBUG_BUFFER_HEX,
	DEBUG_BUFFER_ASCII,
	DEBUG_BUFFER_DECIMAL,
}DebugBuffer_t;

void DEBUG_Init(void);

void DEBUG_DispStr(char *str);
void DEBUG_DispBuffer(DebugBuffer_t type, uint8_t *pData, uint16_t length, uint8_t flag);
void DEBUG_DispError(char *msg,char *file, uint32_t line);

#endif