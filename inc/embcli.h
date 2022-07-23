#ifndef __CLI_DEVICE_HEADER__
#define __CLI_DEVICE_HEADER__

#include "stdint.h"

#ifndef NULL
    #define NULL 	((void *)0)
#endif

#define CLI_HEADER_LENGTH_FIELD_SIZE     1
#define CLI_HEADER_GROUP_FIELD_SIZE      1
#define CLI_HEADER_ID_FIELD_SIZE         1
#define CLI_HEADER_STATUS_FIELD_SIZE     1

#define CLI_INPUT_HEADER_SIZE            CLI_HEADER_LENGTH_FIELD_SIZE + \
                                         CLI_HEADER_GROUP_FIELD_SIZE  + \
                                         CLI_HEADER_ID_FIELD_SIZE

#define CLI_OUTPUT_HEADER_SIZE           CLI_HEADER_LENGTH_FIELD_SIZE + \
                                         CLI_HEADER_STATUS_FIELD_SIZE

#define CLI_BUF_SIZE 100

typedef enum
{
    CLI_STATUS_SUCCESS = 0,
    CLI_STATUS_FAIL,
    CLI_STATUS_ERROR_WRONG_HEADER,
    CLI_STATUS_ERROR_CMD_NOT_EXIST,
} cli_status;

#ifdef UNIT_TEST
    #pragma pack(1)
#endif

typedef struct __attribute__((packed))
{
    void (*pSerialTxChar_func)(uint8_t);
    void (*pSerialRxChar_func)(uint8_t *);
} cli_cfg;

typedef struct __attribute__((packed))
{
    uint8_t         group;
    uint8_t         id;
    uint8_t         data[CLI_BUF_SIZE];    
} cli_cmd;

typedef struct __attribute__((packed))
{
    uint8_t         length;
    uint8_t         data[CLI_BUF_SIZE];    
} cli_rsp;

typedef struct __attribute__((packed))
{
    uint8_t         length;
    cli_cmd         command;   
} cli_in;

typedef struct __attribute__((packed))
{
    cli_status      status;
    cli_rsp         response;   
} cli_out;

typedef struct __attribute__((packed))
{
    uint8_t group;
    uint8_t id;
    cli_status (*func)(uint8_t *pInBuf, cli_rsp *pOutRsp);    
} cli_cmd_vector;

#ifdef UNIT_TEST
    #pragma pack()
#endif

extern cli_cmd_vector CmdTable[10];

cli_status cli_init(cli_cfg *pCfg);
cli_status cli_receive_cmd(uint8_t *pInBuf);
cli_status cli_command_handler(cli_in *pInString, cli_out *pOutString);
void       cli_transmit_rsp(cli_out *pOutString);
cli_status cli_transceive(uint8_t *pInBuf, uint8_t *pOutBuf);

#endif
