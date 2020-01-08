
#include "serial.h"
#include"spi.h"
#include "r_cg_macrodriver.h"
//Setting for SPI Communication with mode 0 channel 3 
void R_SAU0_Create(void){
    SAU0EN = 1U;/* enables input clock supply*/
    SPS0 =   _0000_SAU_CK00_fCLK_0;
}
void R_SPI_Initial(void){
/* Set SPI Mode 3 of channel 0*/
SS0 |= _0008_SAU_CH3_START_TRG_ON|_0000_SAU_CH3_START_TRG_OFF;

/*setting the PER0 register*/
PER0 = _00_Peripheral_enable_fclk_0;
/* setting the SPS0 register*/
SPS0 |= _0004_SAU_CK00_fCLK_4| _0000_SAU_CK00_fCLK_0;
/*setting the SMR03 register*/
SMR03 = _0020_SAU_SMR03_DEFAULT_VALUE;
/*setting the SCR03 register*/
SCR03 = _8016_SCR00_SETTING_VALUE;
/*setting the SDR register */
SDR03 = _CE00_SAU1_CH0_BAUDRATE_DIVISOR;
/*Register Serial Output SOm */
SO0 |= _0008_SAU_CH0_SO3_DATA_OUT;
/*Changing  Setting of the SOEm Register */
SOE0 |= _0008_SAU_CH3_OUTPUT_ENABLE;
/* Set SI11 Pin */
PM1|=0x01U; //Set bit0 to 1
/* Set SO Pin*/
//PM2|=0x02U;//Set bits 1 to 1





}
/*void PortInit(void){
	PM1.1 = 1;			//Set pin at Input SI
	
	PM1.7 = 0;
	PMC14.7 = 0;
	
	P6.2 = 1;
	P14.7 = 1;
}
*/