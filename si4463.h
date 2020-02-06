#ifndef __SI4463_H__
#define __SI4463_H__
#include "user_def.h"
#include "r_cg_macrodriver.h"
#include"radio_config.h"
#include"SPISOF.h"
#include"si4463_defs.h"
#define SI446X_PROP_GROUP_PA			0x22
typedef enum
{
	SI446X_STATE_NOCHANGE	= 0x00,
	SI446X_STATE_SLEEP		= 0x01, ///< This will never be returned since SPI activity will wake the radio into ::SI446X_STATE_SPI_ACTIVE
	SI446X_STATE_SPI_ACTIVE	= 0x02,
	SI446X_STATE_READY		= 0x03,
	SI446X_STATE_READY2		= 0x04, ///< Will return as ::SI446X_STATE_READY
	SI446X_STATE_TX_TUNE	= 0x05, ///< Will return as ::SI446X_STATE_TX
	SI446X_STATE_RX_TUNE	= 0x06, ///< Will return as ::SI446X_STATE_RX
	SI446X_STATE_TX			= 0x07,
	SI446X_STATE_RX			= 0x08
} si446x_state_t;


#define SI446X_MAX_TX_POWER               127
//
// Developed by Alexander Sholohov <ra9yer@yahoo.com>
//

void Si4463_init(void);
void Si4463_setTxPower(uint8_t pwr);
uint8_t  si4463_TX(uint8_t *packet, uint8_t len,uint8_t channel, uint8_t onTxFinish);
void si446x_RX(uint8_t *data_revceive, uint8_t len,uint8_t channel);


#endif