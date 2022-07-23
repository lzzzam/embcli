#include "onChip.h"
#include "onChip_Cmd.h"

static void (*onChip_read_char)(uint8_t *data);
static void (*onChip_write_char)(uint8_t data);

onChip_Status onChip_init(onChip_Cfg *pCfg)
{
    if ((pCfg->pSerialRxChar_func == NULL) || 
        (pCfg->pSerialTxChar_func == NULL))
    {
        return ONCHIP_STATUS_FAIL;
    }
    
    onChip_read_char  = pCfg->pSerialRxChar_func;
    onChip_write_char = pCfg->pSerialTxChar_func;

    return ONCHIP_STATUS_SUCCESS;
}

onChip_Status onChip_receive_cmd(uint8_t *pInBuf)
{
    uint8_t data;
    uint8_t counter = 0;
    uint8_t length;
    uint8_t length_received = 0;

    do
    {
        // Read new data (blocking)
        onChip_read_char(&data);

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
    if (counter < ONCHIP_INPUT_HEADER_SIZE)
    {
        return ONCHIP_STATUS_ERROR_WRONG_HEADER;
    }

    return ONCHIP_STATUS_SUCCESS;
}

void onChip_transmit_rsp(onChip_out *pOutString)
{
    uint8_t length = pOutString->response.length;
    uint8_t *buf = pOutString->response.data;
    onChip_Status status = pOutString->status;
   
    // Write length = (response data length + status field + length field)
    onChip_write_char(ONCHIP_OUTPUT_HEADER_SIZE + length);

    // Write status
    onChip_write_char((uint8_t)status);

    // Write data payload
    for(uint8_t i=0; i< length; i++)
    {
        onChip_write_char(buf[i]);
    }
}

onChip_Status onChip_command_handler(onChip_in *pInString, onChip_out *pOutString)
{
    uint8_t cmdFound = 0;
    
    // Search from command Group & ID
    uint8_t group = pInString->command.group;
    uint8_t id    = pInString->command.id;

    for (uint8_t i=0; i<10; i++)
    {
        if ((group == CmdTable[i].group) && (id == CmdTable[i].id))
        {
            // Call command and pass input data and output struct
            pOutString->status = CmdTable[i].func(pInString->command.data, 
                                                  &(pOutString->response));

            // Updated flag
            cmdFound = 1;

            break;
        }
    } 

    // Check if command exist
    if (!cmdFound)
    {
        pOutString->status = ONCHIP_STATUS_ERROR_CMD_NOT_EXIST;
        pOutString->response.length = 0;

        return ONCHIP_STATUS_ERROR_CMD_NOT_EXIST;
    }

    return ONCHIP_STATUS_SUCCESS;
}

onChip_Status onChip_transceive(uint8_t *pInBuf, uint8_t *pOutBuf)
{
    onChip_Status status;

    // Listen for new command
    status = onChip_receive_cmd(pInBuf);

    // Decode and execute
    if (status == ONCHIP_STATUS_SUCCESS)
    {
        status = onChip_command_handler((onChip_in *)pInBuf, (onChip_out *)pOutBuf);
    }
    else
    {
        // In case of error format response
        ((onChip_out *)pOutBuf)->status = status;
        ((onChip_out *)pOutBuf)->response.length = 0;
    }

    // Send response
    onChip_transmit_rsp(((onChip_out *)pOutBuf));

    return status;
}