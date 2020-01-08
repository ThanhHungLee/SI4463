#include "debug.h"
#include "uart.h"


static void DEBUG_SendCpltCallback(void);
static void DEBUG_ReceiveCpltCallback(void);

void DEBUG_Init(void){
//	UartInitStruct_t Uart1InitStruct;
//	Uart1InitStruct.baudrate = UART_BAUDRATE_19200;
//	Uart1InitStruct.parity = UART_PARITY_NONE;
//	Uart1InitStruct.dataLength = UART_DATA_LENGTH_8;
//	Uart1InitStruct.stopBit	= UART_STOP_BIT_1;
//	Uart1InitStruct.SendCallback = &DEBUG_SendCpltCallback;
//	Uart1InitStruct.ReceiveCallback = &DEBUG_ReceiveCpltCallback;
	
	SERIAL_InitUart1();
	SERIAL_StartUart1();
}

void DEBUG_DispStr(char *str){
	
}

void DEBUG_DispBuffer(DebugBuffer_t type, uint8_t *pData, uint16_t length, uint8_t flag){
	
}

void DEBUG_DispError(char *msg,char *file, uint32_t line){
	
}

static void DEBUG_SendCpltCallback(void){
	
}

static void DEBUG_ReceiveCpltCallback(void){
	
}

