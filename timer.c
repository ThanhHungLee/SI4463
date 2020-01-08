#pragma interrupt INTTM00 TAU0_Channel0_Interrupt
#pragma interrupt INTTM01 TAU0_Channel1_Interrupt
#pragma interrupt INTTM02 TAU0_Channel2_Interrupt
#pragma interrupt INTTM03 TAU0_Channel3_Interrupt


#include "timer.h"
#include "user_def.h"



static void supply_clk_timer(void);
static void timer0_init(void);
static void timer1_init(void);
static void timer2_init(void);
static void timer3_init(void);
__interrupt static void TAU0_Channel0_Interrupt(void);
__interrupt static void TAU0_Channel1_Interrupt(void);
__interrupt static void TAU0_Channel2_Interrupt(void);
__interrupt static void TAU0_Channel3_Interrupt(void);

uint32_t sys_tick = 0;

void timer_init(void)
{
	supply_clk_timer();
	timer0_init();
	timer1_init();
	timer2_init();
	timer3_init();
}

uint32_t get_sys_tick(void)
{
	return sys_tick;
}

static void supply_clk_timer(void)
{
	TAU0EN = 1;      /* Supplies input clock */
}

static void timer0_init(void)
{
	/* f-CLK = 32MHz
		CK00: bit 0-3 of TPS0 register
		CK01: bit 4-7 of TPS0 register
		CK02: bit 8-9 of TPS0 register
		CK03: bit 12-13 of TPS0 register
	*/
	TPS0 = 0x0010;   /* CK01 = 2MHz, CK00 = 32MHz, CK02 = 16MHz, CK03 = 125kHz*/
		
	/* Configures TAU00(1ms cycle interrupt) */
	TMR00 = 0x0000;  /* Selection of operation clock CK00(32MHz), interval timer mode */
	TDR00 = 0x7cff;  /* 1ms = 1/32M*(31999+1) */
	TMMK00 = 0;      /* INTTM00 interrupt servicing enabled */
}

static void timer1_init(void)
{
	/* f-CLK = 32MHz
		CK00: bit 0-3 of TPS0 register
		CK01: bit 4-7 of TPS0 register
		CK02: bit 8-9 of TPS0 register
		CK03: bit 12-13 of TPS0 register
	*/
	TPS0 = 0x0020;   /* CK01 = 2MHz, CK00 = 32MHz, CK02 = 16MHz, CK03 = 125kHz*/
		
	/* Configures TAU00(1ms cycle interrupt) */
	TMR01 = 0x0000;  /* Selection of operation clock CK00(32MHz), interval timer mode */
	TDR01 = 0x7cff;  /* 1ms = 1/32M*(31999+1) */
	TMMK01 = 0;      /* INTTM00 interrupt servicing enabled */
}

static void timer2_init(void)
{
	/* f-CLK = 32MHz
		CK00: bit 0-3 of TPS0 register
		CK01: bit 4-7 of TPS0 register
		CK02: bit 8-9 of TPS0 register
		CK03: bit 12-13 of TPS0 register
	*/
	TPS0 = 0x0040;   /* CK01 = 2MHz, CK00 = 32MHz, CK02 = 16MHz, CK03 = 125kHz*/
		
	/* Configures TAU00(1ms cycle interrupt) */
	TMR02 = 0x0000;  /* Selection of operation clock CK00(32MHz), interval timer mode */
	TDR02 = 0x7cff;  /* 1ms = 1/32M*(31999+1) */
	TMMK02 = 0;      /* INTTM00 interrupt servicing enabled */
}

static void timer3_init(void)
{
	/* f-CLK = 32MHz
		CK00: bit 0-3 of TPS0 register
		CK01: bit 4-7 of TPS0 register
		CK02: bit 8-9 of TPS0 register
		CK03: bit 12-13 of TPS0 register
	*/
	TPS0 = 0x0080;   /* CK01 = 2MHz, CK00 = 32MHz, CK02 = 16MHz, CK03 = 125kHz*/
		
	/* Configures TAU00(1ms cycle interrupt) */
	TMR03 = 0x0000;  /* Selection of operation clock CK00(32MHz), interval timer mode */
	TDR03 = 0x7cff;  /* 1ms = 1/32M*(31999+1) */
	TMMK03 = 0;      /* INTTM00 interrupt servicing enabled */
}

__interrupt static void TAU0_Channel0_Interrupt(void)
{
   sys_tick++;
}

__interrupt static void TAU0_Channel1_Interrupt(void)
{
   if(serial_rx_timeout_cplt > 0) serial_rx_timeout_cplt--;
   else {
   	serial_rx_cplt = 1;
	serial_rx_length = serial_rx_index;
	serial_rx_index = 0;
   }
}

__interrupt static void TAU0_Channel2_Interrupt(void)
{
	//tick_cnt++;
}

__interrupt static void TAU0_Channel3_Interrupt(void)
{

}


