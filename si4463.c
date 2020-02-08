#include "si4463.h"
#include "r_cg_macrodriver.h"
#include "string.h"
#include "user_def.h"
static const uint8_t config[] = RADIO_CONFIGURATION_DATA_ARRAY;
 uint8_t waitForResponse();
static void applyStartupConfig(void);
static void doAPI(uint8_t *data, uint8_t len, void *out, uint8_t outLen);
static void radio_reset();
static void setProperties(uint16_t prop, void *values, uint8_t len);
static void setProperty(uint16_t prop, uint8_t value);
#define IDLE_STATE SI446X_IDLE_MODE

void Si4463_init(void)
{
	//radio_reset();
	applyStartupConfig();
}
static void applyStartupConfig(void)
{
	uint8_t buff[17];
	uint16_t j = 0;
	for (j = 0; j < sizeof(config); j++)
	{
		memcpy(buff, &config[j], sizeof(buff));
		doAPI(&buff[1], buff[0], NULL, 0);
		j += buff[0];
		delay_us(20000);
	}
}

static void radio_reset()
{
	SDN = 1; 
	delay_us(300);
	SDN = 0;
	delay_us(7000);
}
 uint8_t waitForResponse()
{
	uint8_t b_CTSValue = 0;
	uint16_t cntErr = 0;
	while (b_CTSValue != 0xFF)
	{
		CS = 0;
		send_byte(0x44);
		b_CTSValue = receiver_byte();
		
		if (b_CTSValue != 0xFF)
		{
			cntErr++;
			CS = 1;
		}
		if(cntErr == 2500 )
		{
			return 0;
			delay_us(1000);
		}
		
	}
	CS = 1;
	delay_us(10);
	return 1;
}
static void doAPI(uint8_t *data, uint8_t len, void *out, uint8_t outLen)
{
	uint8_t i = 0;
	if (waitForResponse()) // Make sure it's ok to send a command
	{
        //delay_us(10);
		CS = 0;
		for (i = 0; i < len; i++)
		{
			send_byte(data[i]);
		}
		CS = 1;
	}
}

void Si4463_setTxPower(uint8_t pwr)
{
	setProperty(SI446X_PA_PWR_LVL, pwr);
}
static void setProperty(uint16_t prop, uint8_t value)
{
	setProperties(prop, &value, 1);
}

// const uint8_t  a = (uint8_t)(SI446X_PA_PWR_LVL>>8);

static void setProperties(uint16_t prop, void *values, uint8_t len)
{
	// len must not be greater than 12
	uint8_t data[16];
	data[0] = SI446X_CMD_SET_PROPERTY;
	data[1] = (uint8_t)(prop >> 8);
	data[2] = len;
	data[3] = (uint8_t)prop;
	data[4] = 0x7f;//*values;
	// uint8_t data[16] = {
	// 	SI446X_CMD_SET_PROPERTY,
	// 	0x00,//(uint8_t)(prop>>8),
	// 	0x01,//len,
	// 	0x01,//(uint8_t)prop
	// };

	// Copy values into data, starting at index 4
	//memcpy(data + 4, values, len);

	doAPI(data, len + 4, NULL, 0);
}

static uint8_t getFRR(uint8_t reg)
{
	uint8_t frr = 0;

		CS = 0;
		send_byte(reg);
	   frr = receiver_byte();
		CS = 1 ;
	
	return frr;
}
static si446x_state_t getState(void)
{
	uint8_t state = getFRR(SI446X_CMD_READ_FRR_B);
	if(state == SI446X_STATE_TX_TUNE)
		state = SI446X_STATE_TX;
	else if(state == SI446X_STATE_RX_TUNE)
		state = SI446X_STATE_RX;
	else if(state == SI446X_STATE_READY2)
		state = SI446X_STATE_READY;
	return (si446x_state_t)state;
}


///transmition packet data

// static void clearFIFO(void)
// {
// 	// 'static const' saves 20 bytes of flash here, but uses 2 bytes of RAM
// 	static const uint8_t clearFifo[] = {
// 		SI446X_CMD_FIFO_INFO,
// 		SI446X_FIFO_CLEAR_RX | SI446X_FIFO_CLEAR_TX
// 	};
// 	doAPI((uint8_t*)clearFifo, sizeof(clearFifo), NULL, 0);
// }

//ready State
static void setState()
{
	uint8_t data[] = {
		SI446X_CMD_CHANGE_STATE,
		0x01};
	doAPI(data, sizeof(data), NULL, 0);
	delay_us(10);
}

static void clearFIFO(void)
{
	// 'static const' saves 20 bytes of flash here, but uses 2 bytes of RAM
	static const uint8_t clearFifo[] = {
		SI446X_CMD_FIFO_INFO,
		SI446X_FIFO_CLEAR_RX | SI446X_FIFO_CLEAR_TX
	};
	doAPI((uint8_t*)clearFifo, sizeof(clearFifo), NULL, 0);
}
static void get_int_status()
{
	uint8_t data[] = {0x20, 0, 0, 0xFF};
	doAPI(data, sizeof(data),NULL,0);
	delay_us(10);
}
// static void get_int_status(void* buff, uint8_t clearPH, uint8_t clearMODEM, uint8_t clearCHIP)
// {
// 	uint8_t data[] = {
// 		SI446X_CMD_GET_INT_STATUS,
// 		clearPH,
// 		clearMODEM,
// 		clearCHIP
// 	};
// 	doAPI(data, sizeof(data), buff, 8);
// }
static void Writedatatobuffer(uint8_t *data, uint8_t len)
{
	uint8_t i = 0;
	CS = 0;
	send_byte(0x66);
	for (i = 0; i < len; i++)
	{
		send_byte(data[i]);
	}
	delay_us(10);
	CS = 1;
	delay_us(10);
}

uint8_t  si4463_TX(uint8_t *packet,  uint8_t len,uint8_t channel,uint8_t onTxFinish)
{
	uint8_t retrieve_byte[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
	//uint8_t Start_tx[] = {0x31, 0x00, 0x80, 0x00, 0x00, 0x00, 0x00};
	uint8_t Start_tx[7]; 
		Start_tx[0]=	SI446X_CMD_START_TX;
		Start_tx[1]=	channel;
		Start_tx[2]=(uint8_t)(onTxFinish<<4);
		Start_tx[3] =0;
		Start_tx[4] =SI446X_FIXED_LENGTH;
		Start_tx[5] =	0;
		Start_tx[6] = 0;
		
	
    if(getState() == SI446X_STATE_TX)
	return 0;
	setState();
	clearFIFO();
	get_int_status();

			
		doAPI(retrieve_byte, sizeof(retrieve_byte), NULL, 0);
		delay_us(10);
		Writedatatobuffer(packet, len);
		doAPI(Start_tx, sizeof(Start_tx), NULL, 0);
	
}
void si446x_RX(uint8_t *data_revceive, uint8_t len,uint8_t channel)
{
	uint8_t i = 0;
	//uint8_t cmd_receive[8] = {0x32, 0, 0, 0, 0, 0, 0, 1};
	uint8_t cmd_receive[8];
		cmd_receive[0]	 = SI446X_CMD_START_RX;
		cmd_receive[1] =	channel;
		cmd_receive[2] =	0;
	    cmd_receive[3] =	0;
		cmd_receive[4] = SI446X_FIXED_LENGTH;
		cmd_receive[5] = SI446X_STATE_NOCHANGE; // RX Timeout
	    cmd_receive[6] =	IDLE_STATE; // RX Valid
		cmd_receive[7] = SI446X_STATE_SLEEP; // IDLE_STATE // RX Invalid (using SI446X_STATE_SLEEP for the INVALID_SYNC fix)
	
	//setState();
	clearFIFO();
	if (waitForResponse())
	{
		{
	//		get_int_status();
			doAPI(cmd_receive, sizeof(cmd_receive), NULL, 0);
		}
		if (waitForResponse())
		{
			CS = 0 ;
			send_byte(0x77);
			for (i = 0; i < len; i++)
			{
				*data_revceive = receiver_byte();
				data_revceive++;
			}
			CS = 1;
		}
	}
}

// uint8_t Si446x_TX(void* packet, uint8_t len, uint8_t channel, si446x_state_t onTxFinish)
// {
// 	// TODO what happens if len is 0?

// #if SI446X_FIXED_LENGTH
// 	// Stop the unused parameter warning
// 	((void)(len));
// #endif

// 	SI446X_NO_INTERRUPT()
// 	{
// 		if(getState() == SI446X_STATE_TX) // Already transmitting
// 			return 0;

// 		// TODO collision avoid or maybe just do collision detect (RSSI jump)

// 		setState(IDLE_STATE);
// 		clearFIFO();
// 		interrupt2(NULL, 0, 0, 0xFF);

// 		SI446X_ATOMIC()
// 		{
// 			// Load data to FIFO
// 			CHIPSELECT()
// 			{
// 				spi_transfer_nr(SI446X_CMD_WRITE_TX_FIFO);
// #if !SI446X_FIXED_LENGTH
// 				spi_transfer_nr(len);
// 				for(uint8_t i=0;i<len;i++)
// 					spi_transfer_nr(((uint8_t*)packet)[i]);
// #else
// 				for(uint8_t i=0;i<SI446X_FIXED_LENGTH;i++)
// 					spi_transfer_nr(((uint8_t*)packet)[i]);
// #endif
// 			}
// 		}

// #if !SI446X_FIXED_LENGTH
// 		// Set packet length
// 		setProperty(SI446X_PKT_FIELD_2_LENGTH_LOW, len);
// #endif

// 		// Begin transmit
// 		uint8_t data[] = {
// 			SI446X_CMD_START_TX,
// 			channel,
// 			(uint8_t)(onTxFinish<<4),
// 			0,
// 			SI446X_FIXED_LENGTH,
// 			0,
// 			0
// 		};
// 		doAPI(data, sizeof(data), NULL, 0);

// #if !SI446X_FIXED_LENGTH
// 		// Reset packet length back to max for receive mode
// 		setProperty(SI446X_PKT_FIELD_2_LENGTH_LOW, MAX_PACKET_LEN);
// #endif
// 	}
// 	return 1;
// }
