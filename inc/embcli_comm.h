#ifndef __CLI_COMM_DEVICE_HEADER__
#define __CLI_COMM_DEVICE_HEADER__

#include "embcli.h"

#define CLI_HEADER_LENGTH_FIELD_SIZE     1
#define CLI_HEADER_GROUP_FIELD_SIZE      1
#define CLI_HEADER_ID_FIELD_SIZE         1
#define CLI_HEADER_STATUS_FIELD_SIZE     1

#define CLI_INPUT_HEADER_SIZE            CLI_HEADER_LENGTH_FIELD_SIZE + \
                                            CLI_HEADER_GROUP_FIELD_SIZE  + \
                                            CLI_HEADER_ID_FIELD_SIZE

#define CLI_OUTPUT_HEADER_SIZE           CLI_HEADER_LENGTH_FIELD_SIZE + \
                                            CLI_HEADER_STATUS_FIELD_SIZE

typedef struct __attribute__((packed))
{
    uint8_t         group;
    uint8_t         id;
    uint8_t         data[CLI_BUF_SIZE];
}
cli_cmd;

typedef struct __attribute__((packed))
{
    uint8_t         length;
    cli_cmd         command;
}
cli_in;

typedef struct __attribute__((packed))
{
    cli_status      status;
    cli_rsp         response;
}
cli_out;

cli_status   cli_receive_cmd(uint8_t *pInBuf);
void         cli_transmit_rsp(cli_out *pOutString);
cli_status   cli_command_handler(cli_in *pInString, cli_out *pOutString);

#endif