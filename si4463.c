#include "si4463.h"
#include "r_cg_macrodriver.h"
#include "string.h"
#include "user_def.h"
static const uint8_t config[] = RADIO_CONFIGURATION_DATA_ARRAY;
static uint8_t waitForResponse();
static void applyStartupConfig(void);
static void doAPI(uint8_t *data, uint8_t len, void *out, uint8_t outLen);
static void radio_reset();
static void setProperties(uint16_t prop, void *values, uint8_t len);
static void setProperty(uint16_t prop, uint8_t value);
#define IDLE_STATE SI446X_IDLE_MODE

void Si4463_init(void)
{
	radio_reset();
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
static uint8_t waitForResponse()
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
			CS = 1;
		}
	}
	return 1;
}
static void doAPI(uint8_t *data, uint8_t len, void *out, uint8_t outLen)
{
	uint8_t i = 0;
	if (waitForResponse()) // Make sure it's ok to send a command
	{

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
	data[4] = 0x7F;
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
		0x03};
	doAPI(data, sizeof(data), NULL, 0);
	delay_us(10);
}
static void clearFIFO()
{
	CS = 0;
	send_byte(0x15);
	send_byte(0x03);
	delay_us(10);
	CS = 1;
	delay_us(10);
}
static void get_int_status()
{
	uint8_t data[] = {0x20, 0, 0, 0xFF};
	uint8_t i = 0;
	CS = 0;
	for (i = 0; i < sizeof(data); i++)
	{
		send_byte(data[i]);
	}
	delay_us(10);
	CS = 1;
	delay_us(10);
}
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

void si4463_TX(uint8_t *packet, uint8_t len)
{
	uint8_t Start_tx[] = {0x31, 0x00, 0x80, 0x00, 0x00, 0x00, 0x00};
	uint8_t retrieve_byte[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
	setState();
	clearFIFO();
	if (waitForResponse())
	{
		get_int_status();
		doAPI(retrieve_byte, sizeof(retrieve_byte), NULL, 0);
		delay_us(10);
		Writedatatobuffer(packet, len);
		doAPI(Start_tx, sizeof(Start_tx), NULL, 0);
	}
}
void si446x_RX(uint8_t *data_revceive, uint8_t len)
{
	uint8_t i = 0;
	uint8_t cmd_receive[8] = {0x32, 0, 0, 0, 0, 0, 0, 1};
	setState();
	clearFIFO();
	if (waitForResponse())
	{
		{
			get_int_status();
			doAPI(cmd_receive, sizeof(cmd_receive), NULL, 0);
		}
		if (waitForResponse())
		{
			for (i = 0; i < len; i++)
			{
				*data_revceive = receiver_byte();
				data_revceive++;
			}
		}
	}
}

uint8_t Si446x_TX(void* packet, uint8_t len, uint8_t channel, si446x_state_t onTxFinish)
{
	// TODO what happens if len is 0?

#if SI446X_FIXED_LENGTH
	// Stop the unused parameter warning
	((void)(len));
#endif

	SI446X_NO_INTERRUPT()
	{
		if(getState() == SI446X_STATE_TX) // Already transmitting
			return 0;

		// TODO collision avoid or maybe just do collision detect (RSSI jump)

		setState(IDLE_STATE);
		clearFIFO();
		interrupt2(NULL, 0, 0, 0xFF);

		SI446X_ATOMIC()
		{
			// Load data to FIFO
			CHIPSELECT()
			{
				spi_transfer_nr(SI446X_CMD_WRITE_TX_FIFO);
#if !SI446X_FIXED_LENGTH
				spi_transfer_nr(len);
				for(uint8_t i=0;i<len;i++)
					spi_transfer_nr(((uint8_t*)packet)[i]);
#else
				for(uint8_t i=0;i<SI446X_FIXED_LENGTH;i++)
					spi_transfer_nr(((uint8_t*)packet)[i]);
#endif
			}
		}

#if !SI446X_FIXED_LENGTH
		// Set packet length
		setProperty(SI446X_PKT_FIELD_2_LENGTH_LOW, len);
#endif

		// Begin transmit
		uint8_t data[] = {
			SI446X_CMD_START_TX,
			channel,
			(uint8_t)(onTxFinish<<4),
			0,
			SI446X_FIXED_LENGTH,
			0,
			0
		};
		doAPI(data, sizeof(data), NULL, 0);

#if !SI446X_FIXED_LENGTH
		// Reset packet length back to max for receive mode
		setProperty(SI446X_PKT_FIELD_2_LENGTH_LOW, MAX_PACKET_LEN);
#endif
	}
	return 1;
}
