#include "Commands_fake.hpp"

cli_cmd_vector CmdTable[10] = \
{
    //Group     Id      Function
    { 0x00,     0x00,   Test0   },
    { 0x00,     0x01,   Test1   },
    { 0x00,     0x02,   Test2   },
    { 0x00,     0x03,   Test3   },
    { 0x00,     0x04,   Test4   },
    { 0x00,     0x05,   Test5   },
    { 0x00,     0x06,   Test6   },
    { 0x00,     0x07,   Test7   },
    { 0x00,     0x08,   Test8   },
    { 0x00,     0x09,   Test9   }
};

Mock<Commands> Commands_mock;

Commands &Commands_fake = Commands_mock.get();

cli_status Test0(uint8_t *pInBuf, cli_rsp *pOutRsp)
{
    return Commands_fake.CmdTest(0, pInBuf, pOutRsp);
}

cli_status Test1(uint8_t *pInBuf, cli_rsp *pOutRsp)
{
    return Commands_fake.CmdTest(1, pInBuf, pOutRsp);
}

cli_status Test2(uint8_t *pInBuf, cli_rsp *pOutRsp)
{
    return Commands_fake.CmdTest(2, pInBuf, pOutRsp);
}

cli_status Test3(uint8_t *pInBuf, cli_rsp *pOutRsp)
{
    return Commands_fake.CmdTest(3, pInBuf, pOutRsp);
}

cli_status Test4(uint8_t *pInBuf, cli_rsp *pOutRsp)
{
    return Commands_fake.CmdTest(4, pInBuf, pOutRsp);
}

cli_status Test5(uint8_t *pInBuf, cli_rsp *pOutRsp)
{
    return Commands_fake.CmdTest(5, pInBuf, pOutRsp);
}

cli_status Test6(uint8_t *pInBuf, cli_rsp *pOutRsp)
{
    return Commands_fake.CmdTest(6, pInBuf, pOutRsp);
}

cli_status Test7(uint8_t *pInBuf, cli_rsp *pOutRsp)
{
    return Commands_fake.CmdTest(7, pInBuf, pOutRsp);
}

cli_status Test8(uint8_t *pInBuf, cli_rsp *pOutRsp)
{
    return Commands_fake.CmdTest(8, pInBuf, pOutRsp);
}

cli_status Test9(uint8_t *pInBuf, cli_rsp *pOutRsp)
{
    return Commands_fake.CmdTest(9, pInBuf, pOutRsp);
}