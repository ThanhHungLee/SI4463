#ifndef __SPI_H__
#define __SPI_H__

#include "user_def.h"
#include "r_cg_macrodriver.h"
//define Clock System
#define _00_SAU_SMR_fclk_0 (0x0000U) 
//define Peripheral Enable Register
#define _00_Peripheral_enable_fclk_0  (0x84U)

//Define Serial Mode Register SM
#define _0020_SAU_SMR00_DEFAULT_VALUE (0x0020U);
#define _0020_SAU_SMR01_DEFAULT_VALUE (0x0020U);
#define _0020_SAU_SMR02_DEFAULT_VALUE (0x0020U);
#define _0020_SAU_SMR03_DEFAULT_VALUE (0x0020U);
//#define _0020_SAU_SMR10_DEFAULT_VALUE (0x0020U);
//#define _0020_SAU_SMR11_DEFAULT_VALUE (0x0020U);

//Define Serial Communication Register SCRmn : No Parity, 1 Stop, 7 Bits Data
#define _8016_SCR00_SETTING_VALUE     (0x8016U)
#define _8016_SCR01_SETTING_VALUE     (0x8016U)
#define _8016_SCR02_SETTING_VALUE     (0x8016U)
#define _8016_SCR03_SETTING_VALUE     (0x8016U)
//#define _8016_SCR10_DEFAULT_VALUE     (0x8016U)
//#define _8016_SCR11_DEFAULT_VALUE     (0x8016U)

//Define The Serial Data Register SDRmn
/*
              ***************************
 
*/
//Define Serial Data Output SOm
#define _0001_SAU_CH0_SO0_DATA_OUT   (0x0001)
#define _0002_SAU_CH0_SO1_DATA_OUT   (0x0002)
#define _0004_SAU_CH0_SO2_DATA_OUT   (0x0004)
#define _0008_SAU_CH0_SO3_DATA_OUT   (0x0008)
//#define _0001_SAU_CH1_SO1_DATA_OUT   (0x0001)
//#define _0001_SAU_CH1_SO1_DATA_OUT   (0x0002)
void R_SAU0_Create(void);
void R_SPI_Initial(void);

#endif