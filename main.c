
#include "user_def.h"
#include "timer.h"
#include "serial.h"
#include "r_cg_macrodriver.h"
#include "SPISOF.h"
#include"si4463.h"

static uint32_t tick_led = 0;
unsigned char data[10];
int i = 0;


////////////////////////////////////////



void PortInit(void)
{
	PM6.2 = 0; //Set pin at output

	PM14.7 = 0;
	PMC14.7 = 0;

	P6.2 = 1;
	P14.7 = 1;
	PORT_MODE_SCK= 0; //  SCK
	PORT_MODE_MOSI = 0; // MOSI
	PORT_MODE_CS = 0; // CS
	PORT_MODE_MISO = 1; //  MISO
	PORT_MODE_SDN = 0; // SDN
	PORT_MODE_GPIO1 = 1;
	PORT_MODE_nIRQ = 1;
	ADPC = 1;
	SCK = 0;
	MOSI = 1;
	CS = 1;
	SDN = 1;
}

void hdwinit(void)
{
	DI(); // Disable Interrupt
	PortInit();
	timer_init();
	R_SAU1_Create();
	EI(); // Enable Interrupt
}
uint8_t b_data_TX[]= {0x05,0x05,0x05,0x05,0x05};
uint8_t b_data_RX[10];
void main(void)
{
	hdwinit();

	/* Enables TAU00, TAU01, TAU02, TAU03 */
	TS0 |= 0x000F;
	R_UART2_Start();
	 Si4463_init();
	
    Si4463_setTxPower(SI446X_MAX_TX_POWER);
	while (1)
	{

 		if ((get_sys_tick() - tick_led) >= 100)
		{
			
			si4463_TX(b_data_TX,sizeof(b_data_TX),0,SI446X_STATE_RX);
			// si446x_RX(b_data_RX, sizeof(b_data_RX),0,SI446X_STATE_RX);
			//R_UART2_Send(" here", 5);
			P14 .7 ^= 1;//chop led
			tick_led = get_sys_tick();
		}
		// if (serial_rx_cplt == 1)
		// {
		// 	R_UART2_Send(serial_rx_buf, serial_rx_length);
		// 	serial_rx_cplt = 0;
		// }
	}
}
