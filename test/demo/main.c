#include "STM32F303RE.h"
#include "system.h"
#include "embcli.h"

cli_status  Test(uint8_t *pInBuf, cli_rsp *pOutRsp);

cli_cmd_vector  CmdTable[10] = \
{
    //Group     Id      Function
    { 0x00,     0x00,   Test   },
    { 0x00,     0x01,   Test   },
    { 0x00,     0x02,   Test   },
    { 0x00,     0x03,   Test   },
    { 0x00,     0x04,   Test   },
    { 0x00,     0x05,   Test   },
    { 0x00,     0x06,   Test   },
    { 0x00,     0x07,   Test   },
    { 0x00,     0x08,   Test   },
    { 0x00,     0x09,   Test   }
};

int main(void)
{	
	uint8_t	inBuf[100];
    uint8_t outBuf[100];

    while(1)
	{
		// Listen till new command is received
		cli_status status = cli_transceive(inBuf, outBuf);;
	}
}

void            USART3_write_char(uint8_t data);
void            USART3_read_char(uint8_t *data);

USART_handle    USART3_handle;
cli_cfg         SerialCfg = {&USART3_write_char, &USART3_read_char};


/* 
 * Wrapper to adapt __USART_write_char interface in order
 * to be properly used by the cli module. 
 * This function is linked to the main read function pointer 
 * during cli_init, see @ref cli_write_char() 
 */
void USART3_write_char(uint8_t data)
{
	__USART_write_char(&USART3_handle, data);
}

/* 
 * Wrapper to adapt __USART_read_char interface in order
 * to be properly used by the cli module. 
 * This function is linked to the main read function pointer 
 * during cli_init, see @ref cli_read_char() 
 */
void USART3_read_char(uint8_t *data)
{
	__USART_read_char(&USART3_handle, data);
}


void systemInit(void)
{	
    // Initialize cli module with the Serial 
	// read\write functions to be used
	cli_init(&SerialCfg);
	
	// Enable GPIO Port B - Peripheral clock
    __GPIO_EnPCLK(GPIOB);

    // Enable USART3 - Peripheral clock
    __USART_EnPCLK(USART3);

    // Configure PB8 as USART3 - RX
	__GPIO_init(GPIOB, 8, GPIO_MODE_AF, GPIO_OTYPE_PP, GPIO_SPEED_LOW, GPIO_NO_PUPD, GPIO_ALT_FNC_7);

    // Configure PB9 as USART3 - TX
	__GPIO_init(GPIOB, 9, GPIO_MODE_AF, GPIO_OTYPE_PP, GPIO_SPEED_LOW, GPIO_NO_PUPD, GPIO_ALT_FNC_7);

    // Initialize USART3 handle
    USART3_handle.pUSARTx = USART3;
    USART3_handle.pUSARTx_config.Speed = 9600;
    USART3_handle.pUSARTx_config.WordLen = USART_WORD_8_BITS;
    USART3_handle.pUSARTx_config.StopBit = USART_1_STOP_BITS;
    USART3_handle.pUSARTx_config.ParityBit = USART_NO_PARITY;

    // Initialize USART3
    __USART_init(&USART3_handle);
}


cli_status Test(uint8_t *pInBuf, cli_rsp *pOutRsp)
{
    pOutRsp->length = pInBuf[0];

    for (uint8_t i=0; i<pOutRsp->length; i++)
    {
        pOutRsp->data[i] = pInBuf[i];
    }

    return CLI_STATUS_SUCCESS;
}