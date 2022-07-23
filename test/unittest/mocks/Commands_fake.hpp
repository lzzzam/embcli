#include "STM32F303RE.h"
#include "embcli.h"
#include "fakeit.hpp"

using namespace fakeit;

struct Commands 
{
   virtual cli_status CmdTest(uint8_t tableEntry, uint8_t *pInBuf, cli_rsp *pOutRsp) = 0;   
};

extern Mock<Commands> Commands_mock;

cli_status Test0(uint8_t *pInBuf, cli_rsp *pOutRsp);
cli_status Test1(uint8_t *pInBuf, cli_rsp *pOutRsp);
cli_status Test2(uint8_t *pInBuf, cli_rsp *pOutRsp);
cli_status Test3(uint8_t *pInBuf, cli_rsp *pOutRsp);
cli_status Test4(uint8_t *pInBuf, cli_rsp *pOutRsp);
cli_status Test5(uint8_t *pInBuf, cli_rsp *pOutRsp);
cli_status Test6(uint8_t *pInBuf, cli_rsp *pOutRsp);
cli_status Test7(uint8_t *pInBuf, cli_rsp *pOutRsp);
cli_status Test8(uint8_t *pInBuf, cli_rsp *pOutRsp);
cli_status Test9(uint8_t *pInBuf, cli_rsp *pOutRsp);