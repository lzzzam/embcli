#include "embcli.h"
#include "embcli_comm.h"

static void         (*cli_read_char)(uint8_t *data);
static void         (*cli_write_char)(uint8_t data);

cli_status cli_init(cli_cfg *pCfg)
{
    if ((pCfg->pReadChar == NULL) || 
        (pCfg->pWriteChar == NULL))
    {
        return CLI_STATUS_FAIL;
    }
    
    cli_read_char  = pCfg->pReadChar;
    cli_write_char = pCfg->pWriteChar;

    return CLI_STATUS_SUCCESS;
}

cli_status cli_receive_cmd(uint8_t *pInBuf)
{
    uint8_t data;
    uint8_t counter = 0;
    uint8_t length;
    uint8_t length_received = 0;

    do
    {
        // Read new data (blocking)
        cli_read_char(&data);

        // Read first byte as length
        if (!length_received)
        {
            length = data;
            length_received = 1;
        }
        
        // Store data into buffer
        pInBuf[counter] = data;
        counter++;

    } while (counter < length);

    // Sanity check on number of received bytes
    if (counter < CLI_INPUT_HEADER_SIZE)
    {
        return CLI_STATUS_ERROR_WRONG_HEADER;
    }

    return CLI_STATUS_SUCCESS;
}

void cli_transmit_rsp(cli_out *pOutString)
{
    uint8_t length = pOutString->response.length;
    uint8_t *buf = pOutString->response.data;
    cli_status status = pOutString->status;
   
    // Write length = (response data length + status field + length field)
    cli_write_char(CLI_OUTPUT_HEADER_SIZE + length);

    // Write status
    cli_write_char((uint8_t)status);

    // Write data payload
    for(uint8_t i=0; i< length; i++)
    {
        cli_write_char(buf[i]);
    }
}

cli_status cli_command_handler(cli_in *pInString, cli_out *pOutString)
{
    uint8_t cmdFound = 0;
    
    // Search from command Group & ID
    uint8_t group = pInString->command.group;
    uint8_t id    = pInString->command.id;

    for (uint8_t i=0; i<CLI_CMD_TABLE_SIZE; i++)
    {
        if ((group == cliCmdTable[i].group) && (id == cliCmdTable[i].id))
        {
            // Call command and pass input data and output struct
            pOutString->status = cliCmdTable[i].func(pInString->command.data, 
                                                  &(pOutString->response));

            // Updated flag
            cmdFound = 1;

            break;
        }
    } 

    // Check if command exist
    if (!cmdFound)
    {
        pOutString->status = CLI_STATUS_ERROR_CMD_NOT_EXIST;
        pOutString->response.length = 0;

        return CLI_STATUS_ERROR_CMD_NOT_EXIST;
    }

    return CLI_STATUS_SUCCESS;
}

cli_status cli_transceive(uint8_t *pInBuf, uint8_t *pOutBuf)
{
    cli_status status;

    // Listen for new command
    status = cli_receive_cmd(pInBuf);

    // Decode and execute
    if (status == CLI_STATUS_SUCCESS)
    {
        status = cli_command_handler((cli_in *)pInBuf, (cli_out *)pOutBuf);
    }
    else
    {
        // In case of error format response
        ((cli_out *)pOutBuf)->status = status;
        ((cli_out *)pOutBuf)->response.length = 0;
    }

    // Send response
    cli_transmit_rsp(((cli_out *)pOutBuf));

    return status;
}