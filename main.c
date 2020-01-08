#include "user_def.h"
#include "string.h"
#include "stdio.h"
#include "timer.h"
#include "serial.h"
#include "spi.h"
#include "swspi.h"
//#include"stdbool.h"
#include "r_cg_macrodriver.h"
#include "SIConfig.h"

uint8_t i = 0, j = 0, k = 0, cnt = 5;
uint16_t Max_CTS_Retry = 2500;
static uint32_t tick_led = 0;
void receiver_byte(uint8_t *b_data);
void keep_clock(uint16_t num);
void SendDataNoResp(uint8_t Data_length, uint8_t *pdata);
void spi_sendcommand(uint8_t CmdLength, uint8_t *pCmddata);
uint8_t GetDataResp(uint8_t length_dataResp, uint8_t *pbDataResp);
uint8_t WaitForCTS_b();
uint8_t b_GetDataresps_send(uint8_t lengthData, uint8_t *pbDataResp);
uint8_t WriteDataTXBuffer(uint8_t b_TxFifoLength, uint8_t *pbTxFifoData);
uint8_t ReadDataRxBuffer(uint8_t b_RxFifolength, uint8_t *pbRxFifoData);
uint8_t SendCmd_CFG(uint8_t CmdLength, uint8_t *Pbcmd, uint32_t xus_delay);
//khoi tao Port cho SPI
void SPI_init(void)
{
	// Set PM cho sck,MOSI,SS is Output
	PORT_MODE_CS = 0;
	PORT_MODE_MOSI = 0;
	PORT_MODE_SCK = 0;
	PORT_MODE_MISO = 1;
	PORT_MODE_nIRQ = 1;
	PORT_MODE_SDN = 0;
	PORT_MODE_GPIO1 = 1;
	SDN = 1;
	SCK = 1;
	MOSI = 1;
	CS = 1;
	//nIRQ = 0;
}
//tao timer delay
void PortInit(void)
{
	PM6 .2 = 0; //Set pin at output
	//PM2.1 = 1;
	PM14 .7 = 0;
	PMC14 .7 = 0;
	P6 .2 = 1;
	P14 .7 = 1;
}
void timer_delay_1us(void)
{
	TAU0EN = 1;
	TPS0 = 0x0010;
	TMR00 = 0X0000;
	TDR00 = 0x7cff;
	TMMK00 = 0;
}
void timer_delay_xus(uint32_t idx)
{
	while (idx)
	{
		timer_delay_1us();
		idx--;
	}
}
//chay xung clock
void clock1(void)
{
	SCK = 1;
	timer_delay_1us();
	SCK = 0;
	timer_delay_1us();
}
void clock(unsigned int input)
{
	SCK = input;
}
// Send 1 byte data,

void  send_byte1(unsigned char data)
{
	unsigned int i = 0, x = 0;
	
	for (i = 0; i < 8; i++)
	{
		clock(0);
		x = data & 0x80;
		if (x > 0)
		{
			MOSI = 1;
			//R_UART2_Send("1",1);
		}
		else
		{
			MOSI = 0;
			//R_UART2_Send("0",1);
		}
		clock(1);
		data = data << 1;
		//clock(1);
	}
	//R_UART2_Send(" ", 1);
//	MOSI = 1;
}
void send_byte(unsigned char data)
{
	unsigned int i = 0, x = 0;
	for (j = 0; j < 8; j++)
		{
			x = data & 0x80;
			if (x > 0)
			{
				MOSI = 1;
				clock(1);
			}
			else
			{
				MOSI = 0;
				clock(1);
			}
			clock(0);
			data = data << 1;
		}
	//R_UART2_Send(" ", 1);
	MOSI = 1;
}

void receiver_byte(uint8_t *b_data)
{
	uint8_t data, i = 0;
	for (i = 0; i < 8; i++)
	{
		clock(0);
		*b_data = *b_data << 1;
		clock(1);
		if (MISO)
		{
			*b_data = *b_data | 0x01;
		}
		else
		{
			*b_data = *b_data | 0x00;
		}

		clock(0);
	} 
}
//function send command to configure SI4463
uint8_t SendCmd_CFG(uint8_t CmdLength, uint8_t *Pbcmd, uint32_t xus_delay)
{

	for (i = 0; i <= CmdLength; i += 2)
	{
		CS = 0;
		timer_delay_xus(xus_delay);
		send_byte(*Pbcmd);
		Pbcmd++;
		send_byte(*Pbcmd);
		Pbcmd++;
		CS = 1;
	}
}

// function send data no respond
void spi_sendcommand(uint8_t CmdLength, uint8_t *pCmddata)
{

	CS = 0;
	SendDataNoResp(CmdLength, pCmddata);
	CS = 1;
	//return 0;
}

//function send n byte data
void SendDataNoResp(uint8_t Data_length, uint8_t *pdata)
{
	uint8_t i = 0, x = 0;
	char data = *pdata;
	for (i = 0; i < Data_length; i++)
	{
		for (j = 0; j < 8; j++)
		{

			x = data & 0x80;
			if (x > 0)
			{
				MOSI = 1;
				clock(1);
			}
			else
			{
				MOSI = 0;
				clock(1);
			}
			clock(0);
			data = data << 1;
		}
		data = *++pdata;
	}
}
//function send command wait respond (wait CTS byte)
uint8_t WaitForCTS_b()
{
	uint8_t b_CTSValue = 0;
	uint16_t cntErr = 0;
	while (b_CTSValue != 0xFF)
	{
		CS = 0;
		send_byte(0x44);
		GetDataResp(1, &b_CTSValue);
		if (b_CTSValue == 0xFF)
		{
			return 0;
		}
		CS = 1;
		timer_delay_xus(200);

		if (++cntErr > Max_CTS_Retry)
		{
			return 1;
		}
	}

	return 0;
}
uint8_t check_MISO(uint8_t times)
{
	CS = 0;
	for (i = 0; i < times; i++)
	{
		if (P2 .1 == 0)
		{
			clock(1);
			clock(0);
		}
	}
	CS = 1;
}
//Function Get n byte data resp
uint8_t b_GetDataresps_send(uint8_t lengthData, uint8_t *pbDataResp)
{
	uint8_t b_CTSValue = 0;
	uint16_t cntErr = 0;
	while (b_CTSValue != 0xFF)
	{
		CS = 0;
		send_byte(0x44);
		GetDataResp(1, &b_CTSValue);
		// if (b_CTSValue)
		// {
		//R_UART2_Send(" Y",2);
		// }

		if (b_CTSValue != 0xFF)
		{
			CS = 1;
		}
		if (++cntErr > Max_CTS_Retry)
		{
			return 1;
		}
	}

	SendDataNoResp(lengthData, pbDataResp);
	CS = 1;
	return 0;
}
// function read command buffer, get CTS value

uint8_t GetDataResp(uint8_t length_dataResp, uint8_t *pbDataResp)
{
	uint8_t check = 0;
	uint8_t i = 0, j = 0;
	for (i = 0; i < length_dataResp; i++)
	{
		for (j = 0; j < 8; j++)
		{
			clock(0);
			*pbDataResp = *pbDataResp << 1;
			clock(1);
			if (MISO)
			{
				*pbDataResp = *pbDataResp | 0x01;
			}
			else
			{
				*pbDataResp = *pbDataResp | 0x00;
			}
			clock(0);
		}
		//	R_UART2_Send(pbDataResp,1);
		pbDataResp++;

		j = 0;
	}
	return 0;
}
//function write data into buffer transmission
uint8_t WriteDataTXBuffer(uint8_t b_TxFifolength, uint8_t *pbTxFifoData)
{
	CS = 0;
	send_byte(0x66);
	SendDataNoResp(b_TxFifolength, pbTxFifoData);
	send_byte(0x31);
	CS = 1;
	return 0;
}
//function read data from buffer Receiver
uint8_t ReadDataRxBuffer(uint8_t b_RxFifolength, uint8_t *pbRxFifoData)
{
	CS = 0;
	send_byte(0x77);
	GetDataResp(b_RxFifolength, pbRxFifoData);
	CS = 1;
	return 0;
}

void hdwinit(void)
{
	DI(); // Disable Interrupt
	PortInit();
	timer_init();
	R_SAU1_Create();
	EI(); // Enable Interrupt
}
// function to keep clock and wait request

uint8_t Retrieve_bytes[] = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
uint8_t Get_stt[] = {0x22, 0xff};
uint8_t data_receive[100];
uint8_t check_resp = 0xFF;
uint8_t buffer_data[] = {0xcc, 0xcc, 0xcc, 0xcc, 0xcc};
void main(void)
{
	TS0 |= 0x000F;
	SPI_init();
	hdwinit();
	R_UART2_Start();
	SDN = 1;
	timer_delay_xus(20000);
	SDN = 0;

	timer_delay_xus(2000000);
	SDN = 1;
	clock(0);
	CS = 0;
	send_byte(0x02);
	send_byte(0xff);
	timer_delay_xus(20000);
	SDN = 0;
	SDN = 1;
	CS = 1;
	CS = 0;
	send_byte(0x00);
	send_byte(0xff);
	timer_delay_xus(2000);
	send_byte(0x01);
	send_byte(0xff);
	timer_delay_xus(2000);
	SDN = 0;
	SDN = 1;
	CS = 1;
	timer_delay_xus(20000);
	SendCmd_CFG(192, POR, 20000);
	SDN = 1;
	timer_delay_xus(16000);
	SendCmd_CFG(32, POR2, 0);
	SDN = 0;
	SDN = 1;
	timer_delay_xus(16000);
	SendCmd_CFG(32, POR2, 0);
	timer_delay_xus(75000);
	SDN = 0;
	WaitForCTS_b();
	CS = 0;
	send_byte(0x22);
	send_byte(0xff);
	CS = 1;
	CS = 0;
	send_byte(0x44);
	receiver_byte(&check_resp);
	R_UART2_Send(&check_resp,1);
	 CS = 1;
	//WaitForCTS_b();
 b_GetDataresps_send(8, Retrieve_bytes);

	timer_delay_xus(20000);
	R_UART2_Send(&check_resp,1);
	R_UART2_Send(" here", 5);
	//R_UART2_Send(&check_resp,1);

	while (1)
	{
		CS = 0;
	send_byte(0x44);
	receiver_byte(&check_resp);
	R_UART2_Send(&check_resp,1);
	 CS = 1;
	 R_UART2_Send("Y", 1);
	//	CS = 0;
	//send_byte1(0x44);
	//	CS = 1
	//	timer_delay_xus(1000); 
	//	R_UART2_Send(" ", 1);
		// timer_delay_xus(20);
		// spi_sendcommand(10, Retrieve_bytes);
		// timer_delay_xus(20);
		//WriteDataTXBuffer();
		// 	   timer_delay_xus(20000);
	}
	
}
