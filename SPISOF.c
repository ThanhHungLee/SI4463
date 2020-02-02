#include "SPISOF.h"
#include "r_cg_macrodriver.h"
#include"user_def.h"


 static uint32_t tick = 0;
void clock(uint8_t bitdata)
{
    SCK = bitdata;
    
}
// void SPI_init(void)
// {
//     PM1.2 = 0;  // chĂ¢n SCK
//     PM1.1 = 0;// ChĂ¢n MOSI
//     PM1.7 = 0; // ChĂ¢n SS
//     PM2.1 = 1;// chĂ¢n MISO 
//     SCK=0;     
//     MOSI=0;
//     CS=1;
// }
void  send_byte(unsigned char data)
{

    unsigned int i = 0, x = 0;
    
    for (i = 0; i < 8; i++)
    {
        clock(0);
        x = data & 0x80;
        if (x > 0)
        {
            MOSI = 1;
        }
        else
        {
            MOSI = 0;
        }
        clock(1);
        data = data << 1;
    }
    clock(0);

}

void send_mulbyte(const uint8_t* send_buf, uint16_t send_num)
{
    unsigned int i = 0;
    while(i<send_num)
    {
            CS=0;
            send_byte(send_buf[i]);
            CS=1;
            i++;
    }
}
//receive byte data function
unsigned char receiver_byte()
{
    unsigned char b_data, i = 0;
    MOSI=1;
    for (i = 0; i < 8; i++)
    {
        clock(0);
        b_data = b_data << 1;
        clock(1);
        if (MISO)
        {
            b_data = b_data | 0x01;
        }
        clock(0);
    } 
    MOSI=0;
    return b_data;
}

void receiver_mulbyte(uint8_t *recdata, uint16_t rec_num)
{
    uint16_t i = 0;
    while(i<rec_num)
    {
        recdata[i] = receiver_byte();
	i++;

    }
}
void SendCmd_CFG(uint8_t CmdLength, uint8_t  *Pbcmd)
{
     uint8_t i = 0;
	for (i = 0; i <= CmdLength; i += 2)
	{
		CS = 0;
        delay_us(20000);
		send_byte(*Pbcmd);
		Pbcmd++;
		send_byte(*Pbcmd);
		Pbcmd++;
		CS = 1;    
	}
}


//function reset 

//send byte data function
 void delay_us(uint32_t idx)
{
	while (idx--)
	{
	 TAU0EN = 1;
	TPS0 = 0x0010;
	TMR00 = 0X0000;
	TDR00 = 0x7cff;
	TMMK00 = 0;
	}
}

