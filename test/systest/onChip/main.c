#include "STM32F303RE.h"
#include "system.h"
#include "embcli.h"
#include "string.h"

cli_status  Echo(uint8_t *pInBuf, cli_rsp *pOutRsp);
cli_status  Led_ON(uint8_t *pInBuf, cli_rsp *pOutRsp);
cli_status  Led_OFF(uint8_t *pInBuf, cli_rsp *pOutRsp);
cli_status  Test(uint8_t *pInBuf, cli_rsp *pOutRsp);
cli_status  Read_Message(uint8_t *pInBuf, cli_rsp *pOutRsp);


cli_cmd_vector  cliCmdTable[CLI_CMD_TABLE_SIZE] = \
{
    //Group     Id      Function
    { 0x00,     0x00,   Echo            },
    { 0x00,     0x01,   Led_ON          },
    { 0x00,     0x02,   Led_OFF         },
    { 0x00,     0x03,   Read_Message    },
    { 0x00,     0x04,   Test            },
    { 0x00,     0x05,   Test            },
    { 0x00,     0x06,   Test            },
    { 0x00,     0x07,   Test            },
    { 0x00,     0x08,   Test            },
    { 0x00,     0x09,   Test            }
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

/****************************************************************
 *          Setup project to properly use library
 ****************************************************************/
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

/*
 * Overload callback to initialize the hardware and embcli
 * module during Startup, this function is called during Reset_Handler.
 */
void systemInit(void)
{
    // Initialize embcli module with the Serial
    // read\write functions to be used
    cli_init(&SerialCfg);

    // Enable GPIO Port A adn B - Peripheral clock
    __GPIO_EnPCLK(GPIOA);
    __GPIO_EnPCLK(GPIOB);

    // Configure PA5 as output for LED driving
    __GPIO_init(GPIOA, 5, GPIO_MODE_OUT, GPIO_OTYPE_PP, GPIO_SPEED_LOW, GPIO_NO_PUPD, GPIO_ALT_FNC_0);

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


/****************************************************************
 *                       TEST FUNCTIONS
 ****************************************************************/

typedef struct
{
    uint8_t length;
    uint8_t data[100];
} TestBuffer;

/**
 * @brief Echo the received input buffer into the output buffer
 *
 * @param pInBuf    byte[0]    : length of input payload
 *                  byte[1:N]  : binary payload
 *
 * @param pOutRsp   byte[0]    : length of output payload
 *                  byte[1:N]  : echo binary payload
 * @return cli_status
 */
cli_status Echo(uint8_t *pInBuf, cli_rsp *pOutRsp)
{
    TestBuffer *inBuf;
    TestBuffer *outBuf;

    inBuf   = (TestBuffer *)pInBuf;
    outBuf  = (TestBuffer *)pOutRsp->data;

    // Copy input buffer length to output buffer
    outBuf->length = inBuf->length;

    // Copy payload to output buffer.payload
    memcpy(outBuf->data, inBuf->data, inBuf->length);

    pOutRsp->length = sizeof(outBuf->length) + outBuf->length;

    return CLI_STATUS_SUCCESS;
}

/**
 * @brief Turn on LED
 *
 * @param pInBuf    not used
 * @param pOutRsp   byte[0]    : length of output payload
 * @return cli_status
 */
cli_status Led_ON(uint8_t *pInBuf, cli_rsp *pOutRsp)
{
    pOutRsp->length = 0;

    // Drive GPIO PA5 High
    __GPIO_writePin(GPIOA, 5, TRUE);

    return CLI_STATUS_SUCCESS;
}

/**
 * @brief Turn off LED
 *
 * @param pInBuf    not used
 * @param pOutRsp   byte[0]    : length of output payload
 * @return cli_status
 */
cli_status Led_OFF(uint8_t *pInBuf, cli_rsp *pOutRsp)
{
    pOutRsp->length = 0;

    // Drive GPIO PA5 Low
    __GPIO_writePin(GPIOA, 5, FALSE);

    return CLI_STATUS_SUCCESS;
}

/**
 * @brief Send a char string
 *
 * @param pInBuf    not used
 * @param pOutRsp   byte[0]    : length of output payload
 *                  byte[1-end]: char string
 * @return cli_status
 */
cli_status Read_Message(uint8_t *pInBuf, cli_rsp *pOutRsp)
{
    char string[] = "Ciao, questo è un messagio!";

    pOutRsp->length = sizeof(string);

    memcpy(pOutRsp->data, string, sizeof(string));

    return CLI_STATUS_SUCCESS;
}

/**
 * @brief Generic Test function to be called from CLI loop
 *
 * @param pInBuf input data buffer to receive data from host
 * @param pOutRsp output struct to send response data to host
 * @return cli_status
 */
cli_status Test(uint8_t *pInBuf, cli_rsp *pOutRsp)
{
    pOutRsp->length = 0;

    return CLI_STATUS_SUCCESS;
}