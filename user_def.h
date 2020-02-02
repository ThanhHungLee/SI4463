#ifndef __USERDEFINE_H__
#define __USERDEFINE_H__

#pragma sfr
#pragma EI
#pragma DI
#pragma NOP

typedef unsigned char uint8_t;
typedef signed char int8_t;
typedef unsigned int uint16_t;
typedef signed int int16_t;
typedef unsigned long uint32_t;
typedef signed long int32_t;

/* Global var */
/* uart */
extern volatile uint8_t serial_rx_buf[256];
extern volatile uint32_t serial_rx_index;
extern volatile uint32_t serial_rx_length;
extern volatile uint8_t serial_rx_timeout_cplt;
extern volatile uint8_t serial_rx_cplt;

 #endif