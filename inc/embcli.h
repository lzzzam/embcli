#ifndef __CLI_DEVICE_HEADER__
#define __CLI_DEVICE_HEADER__

#include "stdint.h"
#include "stddef.h"

#ifndef CLI_BUF_SIZE
#define CLI_BUF_SIZE                 255
#endif

#ifndef CLI_CMD_TABLE_SIZE
#define CLI_CMD_TABLE_SIZE           10
#endif

#ifdef UNIT_TEST
#pragma pack(1)
#endif

typedef enum
{
    CLI_STATUS_SUCCESS = 0,
    CLI_STATUS_FAIL,
    CLI_STATUS_ERROR_WRONG_HEADER,
    CLI_STATUS_ERROR_CMD_NOT_EXIST,
} cli_status;

typedef __attribute__((packed)) struct
{
    void (*pWriteChar)(uint8_t);
    void (*pReadChar)(uint8_t *);
} cli_cfg;

typedef __attribute__((packed)) struct
{
    uint8_t         length;
    uint8_t         data[CLI_BUF_SIZE];
} cli_rsp;

typedef __attribute__((packed)) struct
{
    uint8_t         group;
    uint8_t         id;
    cli_status      (*func)(uint8_t *pInBuf, cli_rsp *pOutRsp);
} cli_cmd_vector;

extern cli_cmd_vector cliCmdTable[CLI_CMD_TABLE_SIZE];

cli_status cli_init(cli_cfg *pCfg);
cli_status cli_transceive(uint8_t *pInBuf, uint8_t *pOutBuf);

#endif
