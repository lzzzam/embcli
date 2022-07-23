#include "STM32F303RE.h"
#include "system.h"
#include "onChip.h"

void USART3_write_char(uint8_t data);
void USART3_read_char(uint8_t *data);

USART_handle USART3_handle;
onChip_Cfg SerialCfg = {&USART3_write_char, &USART3_read_char};
uint8_t	inBuf[100];
uint8_t outBuf[100];

int main(void)
{	
	while(1)
	{
		// Listen till new command is received
		onChip_Status status = onChip_transceive(inBuf, outBuf);;
	}
}

/* 
 * Wrapper to adapt __USART_write_char interface in order
 * to be properly used by the onChip module. 
 * This function is linked to the main read function pointer 
 * during onChip_init, see @ref onChip_write_char() 
 */
void USART3_write_char(uint8_t data)
{
	__USART_write_char(&USART3_handle, data);
}

/* 
 * Wrapper to adapt __USART_read_char interface in order
 * to be properly used by the onChip module. 
 * This function is linked to the main read function pointer 
 * during onChip_init, see @ref onChip_read_char() 
 */
void USART3_read_char(uint8_t *data)
{
	__USART_read_char(&USART3_handle, data);
}


void systemInit(void)
{	
    // Initialize onChip module with the Serial 
	// read\write functions to be used
	onChip_init(&SerialCfg);
	
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
