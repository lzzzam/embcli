#include "onChip_Cmd.h"

onChip_Cmd_Entry CmdTable[10] = \
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

onChip_Status Test(uint8_t *pInBuf, onChip_Rsp *pOutRsp)
{
    pOutRsp->length = pInBuf[0];

    for (uint8_t i=0; i<pOutRsp->length; i++)
    {
        pOutRsp->data[i] = pInBuf[i];
    }

    return ONCHIP_STATUS_SUCCESS;
}