#pragma interrupt INTST2 r_uart2_interrupt_send
#pragma interrupt INTSR2 r_uart2_interrupt_receive

#include "serial.h"
#include "r_cg_macrodriver.h"

volatile uint8_t * gp_uart2_tx_address;        /* uart2 transmit buffer address */
volatile uint16_t  g_uart2_tx_count;           /* uart2 transmit data number */
volatile uint8_t * gp_uart2_rx_address;        /* uart2 receive buffer address */
volatile uint16_t  g_uart2_rx_count;           /* uart2 receive data number */
volatile uint16_t  g_uart2_rx_length;          /* uart2 receive data length */


volatile uint8_t serial_rx_buf[256];
volatile uint32_t serial_rx_index = 0;
volatile uint32_t serial_rx_length = 0;
volatile uint8_t serial_rx_timeout_cplt = 0;
volatile uint8_t serial_rx_cplt = 0;


void R_SAU1_Create(void)
{
    /* regbitset SAU1EN=1 */
    SAU1EN = 1U;    /* enables input clock supply */
    NOP();
    NOP();
    NOP();
    NOP();
    /* regset SPS1 */
    SPS1 = _0040_SAU_CK01_fCLK_4 | _0004_SAU_CK00_fCLK_4;
    R_UART2_Create();
}

void R_UART2_Create(void)
{
    /* regset ST11=1|ST10=1 opmode=byvalue */
    ST1 |= _0002_SAU_CH1_STOP_TRG_ON | _0001_SAU_CH0_STOP_TRG_ON;
    /* intpregbitset INTST2 disable */
    STMK2 = 1U;     /* disable INTST2 interrupt */
    STIF2 = 0U;     /* clear INTST2 interrupt flag */
    /* intpregbitset INTSR2 disable */
    SRMK2 = 1U;     /* disable INTSR2 interrupt */
    SRIF2 = 0U;     /* clear INTSR2 interrupt flag */
    /* intpregbitset INTSRE2 disable */
    SREMK2 = 1U;    /* disable INTSRE2 interrupt */
    SREIF2 = 0U;    /* clear INTSRE2 interrupt flag */
    /* intpregbitset INTSR2 INTP=PR */
    /* Set INTSR2 low priority */
    SRPR12 = 1U;
    SRPR02 = 1U;
    /* intpregbitset INTSRE2 INTP=PR */
    /* intpregbitset INTST2 INTP=PR */
    /* Set INTST2 low priority */
    STPR12 = 1U;
    STPR02 = 1U;
    /* regset SMR10 */
    SMR10 = _0020_SMR10_DEFAULT_VALUE | _0000_SAU_CLOCK_SELECT_CK00 | _0000_SAU_CLOCK_MODE_CKS | 
            _0002_SAU_MODE_UART | _0000_SAU_TRANSFER_END;
    /* regset SCR10 tag=RECEIVE */
    SCR10 = _0004_SCR10_DEFAULT_VALUE | _8000_SAU_TRANSMISSION | _0000_SAU_TIMING_1 | _0000_SAU_INTSRE_MASK | 
            _0000_SAU_PARITY_NONE | _0080_SAU_LSB | _0010_SAU_STOP_1 | _0003_SAU_LENGTH_8;
    /* regset SDR10 */
    SDR10 = _CE00_SAU1_CH0_BAUDRATE_DIVISOR;
    /* regset SNFEN20=1 opmode=byvalue */
    NFEN0 |= _10_SAU_RXD2_FILTER_ON;
    /* regset FECT11=1|PECT11=1|OVCT11=1 */
    SIR11 = _0004_SAU_SIRMN_FECTMN | _0002_SAU_SIRMN_PECTMN  | _0001_SAU_SIRMN_OVCTMN ;
    /* regset SMR11 */
    SMR11 = _0020_SMR11_DEFAULT_VALUE | _0000_SAU_CLOCK_SELECT_CK00 | _0000_SAU_CLOCK_MODE_CKS | 
            _0100_SAU_TRIGGER_RXD | _0000_SAU_EDGE_FALL | _0002_SAU_MODE_UART | _0000_SAU_TRANSFER_END;
    /* regset SCR11 */
    SCR11 = _0004_SCR11_DEFAULT_VALUE | _4000_SAU_RECEPTION | _0000_SAU_TIMING_1 | _0000_SAU_INTSRE_MASK | 
            _0000_SAU_PARITY_NONE | _0080_SAU_LSB | _0010_SAU_STOP_1 | _0003_SAU_LENGTH_8;
    /* regset SDR11 */
    SDR11 = _CE00_SAU1_CH1_BAUDRATE_DIVISOR;
    /* regset SO10 opmode=byvalue */
    SO1 |= _0001_SAU_CH0_DATA_OUTPUT_1;
    /* regset SOL10 opmode=byvalue */
    SOL1 &= (uint16_t)~_0001_SAU_CHANNEL0_INVERTED;
    /* regset SOE10=1 opmode=byvalue */
    SOE1 |= _0001_SAU_CH0_OUTPUT_ENABLE;
    /* Set RxD2 pin */
    PM1 |= 0x10U;       /* set bit4 to 1 */
    /* Set TxD2 pin */
    P1 |= 0x08U;        /* set bit3 to 1 */
    PM1 &= 0xF7U;       /* set bit3 to 0 */
}

void R_UART2_Start(void)
{
    /* regset SO10 opmode=byvalue */
    SO1 |= _0001_SAU_CH0_DATA_OUTPUT_1;
    /* regset SOE10=1 opmode=byvalue */
    SOE1 |= _0001_SAU_CH0_OUTPUT_ENABLE;
    /* regset SS11=1|SS10=1 opmode=byvalue */
    SS1 |= _0002_SAU_CH1_START_TRG_ON | _0001_SAU_CH0_START_TRG_ON;
    /* intpregbitset INTST2 intp=IF,bitvalue=0 */
    STIF2 = 0U;     /* clear INTST2 interrupt flag */
    /* intpregbitset INTSR2 intp=IF,bitvalue=0 */
    SRIF2 = 0U;     /* clear INTSR2 interrupt flag */
    /* intpregbitset INTST2 intp=MK,bitvalue=0 */
    STMK2 = 0U;     /* enable INTST2 interrupt */
    /* intpregbitset INTSR2 intp=MK,bitvalue=0 */
    SRMK2 = 0U;     /* enable INTSR2 interrupt */
}

void R_UART2_Stop(void)
{
    /* intpregbitset INTST2 intp=MK,bitvalue=1 */
    STMK2 = 1U;     /* disable INTST2 interrupt */
    /* intpregbitset INTSR2 intp=MK,bitvalue=1 */
    SRMK2 = 1U;     /* disable INTSR2 interrupt */
    /* regset ST11=1|ST10=1 opmode=byvalue */
    ST1 |= _0002_SAU_CH1_STOP_TRG_ON | _0001_SAU_CH0_STOP_TRG_ON;
    /* regset SOE10=0 opmode=byvalue */
    SOE1 &= (uint16_t)~_0001_SAU_CH0_OUTPUT_ENABLE;
    /* intpregbitset INTST2 intp=IF,bitvalue=0 */
    STIF2 = 0U;     /* clear INTST2 interrupt flag */
    /* intpregbitset INTSR2 intp=IF,bitvalue=0 */
    SRIF2 = 0U;     /* clear INTSR2 interrupt flag */
}

MD_STATUS R_UART2_Receive(uint8_t * const rx_buf, uint16_t rx_num)
{
    MD_STATUS status = MD_OK;

    if (rx_num < 1U)
    {
        status = MD_ARGERROR;
    }
    else
    {
        g_uart2_rx_count = 0U;
        g_uart2_rx_length = rx_num;
        gp_uart2_rx_address = rx_buf;
    }

    return (status);
}

MD_STATUS R_UART2_Send(uint8_t * const tx_buf, uint16_t tx_num)
{
    MD_STATUS status = MD_OK;

    if (tx_num < 1U)
    {
        status = MD_ARGERROR;
    }
    else
    {
        gp_uart2_tx_address = tx_buf;
        g_uart2_tx_count = tx_num;
        STMK2 = 1U;    /* disable INTST2 interrupt */
        TXD2 = *gp_uart2_tx_address;
        gp_uart2_tx_address++;
        g_uart2_tx_count--;
        STMK2 = 0U;    /* enable INTST2 interrupt */
    }

    return (status);
}

__interrupt static void r_uart2_interrupt_receive(void)
{
    uint8_t rx_data;
    uint8_t err_type;
    
    err_type = (uint8_t)(SSR11 & 0x0007U);
    SIR11 = (uint16_t)err_type;

    if (err_type != 0U)
    {
        r_uart2_callback_error(err_type);
    }
    
    rx_data = RXD2;
	
	serial_rx_buf[serial_rx_index++] = rx_data;
	serial_rx_timeout_cplt = 20;

	/*
    if (g_uart2_rx_length > g_uart2_rx_count)
    {
        *gp_uart2_rx_address = rx_data;
        gp_uart2_rx_address++;
        g_uart2_rx_count++;

        if (g_uart2_rx_length == g_uart2_rx_count)
        {
            r_uart2_callback_receiveend();
        }
    }
    else
    {
        r_uart2_callback_softwareoverrun(rx_data);
    }
	*/
}

__interrupt static void r_uart2_interrupt_send(void)
{
    if (g_uart2_tx_count > 0U)
    {
        TXD2 = *gp_uart2_tx_address;
        gp_uart2_tx_address++;
        g_uart2_tx_count--;
    }
    else
    {
        r_uart2_callback_sendend();
    }
}

static void r_uart2_callback_receiveend(void)
{
    /* Start user code. Do not edit comment generated here */
    /* End user code. Do not edit comment generated here */
}

static void r_uart2_callback_softwareoverrun(uint16_t rx_data)
{
    /* Start user code. Do not edit comment generated here */
    /* End user code. Do not edit comment generated here */
}

static void r_uart2_callback_sendend(void)
{
    /* Start user code. Do not edit comment generated here */
    /* End user code. Do not edit comment generated here */
}

static void r_uart2_callback_error(uint8_t err_type)
{
    /* Start user code. Do not edit comment generated here */
    /* End user code. Do not edit comment generated here */
}
