#define CATCH_CONFIG_MAIN  // This tells Catch to provide a main() - only do this in one cpp file
#include "catch.hpp"
#include "USART_fake.hpp"
#include "Commands_fake.hpp"
#include "embcli.h"

TEST_CASE("Test cli_init") 
{
    cli_cfg cfg;

    SECTION("Pass with correct argument")
    {
        cfg = {USART_write_char, USART_read_char};
        REQUIRE(CLI_STATUS_SUCCESS == cli_init(&cfg));
    }

    SECTION("Test null argument")
    {
        cfg = {NULL, USART_read_char};
        REQUIRE(CLI_STATUS_FAIL == cli_init(&cfg));
    }

    SECTION("Test null argument")
    {
        cfg = {USART_write_char, NULL};
        REQUIRE(CLI_STATUS_FAIL == cli_init(&cfg));
    }
}

TEST_CASE("Test cli_receive_cmd") 
{
    cli_cfg cfg;
    cli_status status;
    uint8_t buf[100];

    USART_mock.Reset();
    
    // init cli module
    cfg = {USART_write_char, \
           USART_read_char};

    cli_init(&cfg);

    SECTION("Receive string with length=6")
    {   
        // Mock to receive length + endCmd char
        When(Method(USART_mock, read_char)).Return(0x06,0xFF,0xFF,0xFF,0xFF,0xFF,'\n');

        // Receive command
        status = cli_receive_cmd(buf);
        
        // Check for success and length matching
        REQUIRE(CLI_STATUS_SUCCESS == status);
        REQUIRE(0x06 == ((cli_in *)buf)->length);
    }

    SECTION("Receive string with length=100")
    {   
        // Mock to receive length + endCmd char
        When(Method(USART_mock, read_char)).Return(100 ,0x00,0x01,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF, \
                                                   0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF, \
                                                   0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF, \
                                                   0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF, \
                                                   0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF, \
                                                   0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF, \
                                                   0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF, \
                                                   0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF, \
                                                   0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF, \
                                                   0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,'\n');

        // Receive command
        status = cli_receive_cmd(buf);
        
        // Check for success and length matching
        REQUIRE(CLI_STATUS_SUCCESS == status);
        REQUIRE(100 == ((cli_in *)buf)->length);

        // Check that Group\Id was received correctly
        REQUIRE(0x00 == ((cli_in *)buf)->command.group);
        REQUIRE(0x01 == ((cli_in *)buf)->command.id);

        // Checl data field was received correctly
        for (uint8_t i=0; i<97; i++)
        {
            REQUIRE(((cli_in *)buf)->command.data[i] == 0xFF);
        }  
    }

    SECTION("Fail receiving only length field")
    {
        // Mock to receive onyl length field
        When(Method(USART_mock, read_char)).Return(0x01);

        // Receive command
        status = cli_receive_cmd(buf);
        
        // Check for success and length matching
        REQUIRE(CLI_STATUS_ERROR_WRONG_HEADER == status);
    }

    SECTION("Fail receiving only length + group fields")
    {
        // Mock to receive onyl length + groupd
        When(Method(USART_mock, read_char)).Return(0x02, 0x00);

        // Receive command
        status = cli_receive_cmd(buf);
        
        // Check for success and length matching
        REQUIRE(CLI_STATUS_ERROR_WRONG_HEADER == status);
    }
}

TEST_CASE( "Test cli_transmit_rsp") 
{
    cli_cfg cfg;
    cli_status status;
    cli_out outString;

    USART_mock.Reset();
    
    // init cli module
    cfg = {USART_write_char, \
           USART_read_char};

    cli_init(&cfg);

    SECTION("Check various transfer length")
    {
        auto rspLength = GENERATE(0,1,2,5,10,20,100);

        Fake(Method(USART_mock, write_char));
        
        // Init response
        outString.status = CLI_STATUS_SUCCESS;
        outString.response.length = (uint8_t) rspLength;

        // Fill response with data
        for (uint32_t i=0; i<(uint8_t)rspLength; i++)
        {
            outString.response.data[i] = 0xFF;
        }

        // Transfer response
        cli_transmit_rsp(&outString);

        // Check that all bytes have been sent
        Verify(Method(USART_mock, write_char)).Exactly(rspLength+2);

        // Check sequence of transfer
        if (rspLength > 0)
        {
            Verify(Method(USART_mock,write_char).Using(outString.response.length +2) + \
                   Method(USART_mock,write_char).Using(outString.status)             + \
                   Method(USART_mock,write_char).Using(0xFF) * rspLength).Exactly(1);
        }
        else //rspLength == 0
        {
            Verify(Method(USART_mock,write_char).Using(2) + \
                   Method(USART_mock,write_char).Using(outString.status)).Exactly(1);
        }

        // Check that no other write have been made
        VerifyNoOtherInvocations(Method(USART_mock,write_char));
    }
}

TEST_CASE( "Test cli_command_handler") 
{
    cli_cfg cfg;
    cli_status status;
    cli_in  inString;
    cli_out outString;
    uint8_t i;

    USART_mock.Reset();
    Commands_mock.Reset();
    
    // init cli module
    cfg = {USART_write_char, \
           USART_read_char};

    cli_init(&cfg);

    SECTION("Call all entries in the CmdTable")
    {
        /* Test all commands in the CmdTable 
         * with various input length */
        auto ID     = GENERATE(0,1,2,3,4,5,6,7,8,9);
        auto LENGTH = GENERATE(take(10, random(0,99)));
        
        // Mock the Test<ID> to copy input buffer into output buffer
        When(Method(Commands_mock, CmdTest)).Do([](uint8_t n, uint8_t *pInBuf, cli_rsp *pOutRsp)->cli_status
        {  
            // Read input length 
            uint8_t len = pInBuf[0];

            // Invert input data to output
            for (uint8_t i=1; i<=len; i++)
            {
                pOutRsp->data[i] = ~pInBuf[i];
            }
            
            // Update response length
            pOutRsp->length  = len;

            // Update response status
            return CLI_STATUS_SUCCESS;
        });

        // Init input string to call "Test<ID>"
        inString.command.group = 0x00;
        inString.command.id    = ID;

        // First byte is length
        inString.command.data[0] = LENGTH;

        // Fill input buffer with random data
        for (i=1; i<= LENGTH; i++)
        {
            inString.command.data[i] = (uint8_t)random();
        }

        // Decode input string and call handler
        cli_command_handler(&inString, &outString);

        // Check that "Test<ID>" was called with right parameters
        Verify(Method(Commands_mock, CmdTest).Using((uint8_t)ID, (uint8_t *)&inString.command.data[0], (cli_rsp *)&outString.response)).Exactly(1);

        // Check response and status returned
        REQUIRE(outString.response.length  == LENGTH);
        REQUIRE(outString.status == CLI_STATUS_SUCCESS);
        for (i=1; i<=LENGTH; i++)
        {
            REQUIRE(outString.response.data[i] == (uint8_t)~inString.command.data[i]);
        }
    }

    SECTION("Check command not found")
    {
        /* Generate Group\Id pairs that 
         * are NOT present in the CmdTable */
        auto GROUP  = GENERATE(1,2,3,4,5,6,7,8,9);
        auto ID     = GENERATE(0,1,2,3,4,5,6,7,8,9);

        // Init input command
        inString.command.group = GROUP;
        inString.command.id    = ID;

        // Fake the Test<ID>
        Fake(Method(Commands_mock, CmdTest));

        // Decode input string and call handler
        cli_command_handler(&inString, &outString);

        // Check if no function was called 
        VerifyNoOtherInvocations(Method(Commands_mock, CmdTest));

        // Check response and status returned
        REQUIRE(outString.status == CLI_STATUS_ERROR_CMD_NOT_EXIST);
        REQUIRE(outString.response.length == 0);
    }
}


TEST_CASE( "Test cli_transceive") 
{
    cli_cfg cfg;
    cli_status status;
    uint8_t inBuf[100];
    uint8_t outBuf[100];

    // Reset mocks
    USART_mock.Reset();
    Commands_mock.Reset();
    
    // init cli module
    cfg = {USART_write_char, USART_read_char};
    cli_init(&cfg);

    SECTION("Call all entries in the CmdTable")
    {
        // Test all command in the CmdTable
        uint8_t GROUP = 0x00;
        auto    ID    = GENERATE(0,1,2,3,4,5,6,7,8,9);
        
        // Trigger command assigned to {Group=0, Id = <ID>} with no data
        When(Method(USART_mock, read_char)).Return(0x03, GROUP, (uint8_t)ID);
        Fake(Method(USART_mock, write_char));

        // Mock all test function to return with success and 0 length
        When(Method(Commands_mock, CmdTest)).Do([](uint8_t n, uint8_t *pInBuf, cli_rsp *pOutRsp)->cli_status
        {
            pOutRsp->length = 0;
            return CLI_STATUS_SUCCESS;
        });

        // Test complete receive + decode + transmit
        status = cli_transceive(inBuf, outBuf);

        // Check that return status is OK
        REQUIRE(status == CLI_STATUS_SUCCESS);

        // Check that "Test<ID>" was called with right parameters
        Verify(Method(Commands_mock, CmdTest).Using((uint8_t) ID, &((cli_in *)inBuf)->command.data[0], &((cli_out *)outBuf)->response)).Exactly(1);

        // Check that only length + status was trasnmitted
        Verify(Method(USART_mock,write_char)).Exactly(2);

        // Check that length + status was transmitted in the right order
        Verify(Method(USART_mock,write_char).Using(((cli_out *)outBuf)->response.length +2) + \
               Method(USART_mock,write_char).Using(((cli_out *)outBuf)->status)).Exactly(1);

        // Check response and status returned
        REQUIRE( ((cli_out *)outBuf)->response.length == 0);
        REQUIRE( ((cli_out *)outBuf)->status == CLI_STATUS_SUCCESS);
    }

    SECTION("Copy input string data to output response")
    {
        // Test all command in the CmdTable
        uint8_t GROUP = 0x00;
        auto    ID    = GENERATE(0,1,2,3,4,5,6,7,8,9);

        uint8_t len  = 50;
        
        // Trigger command assigned to {Group=0, Id = <ID>} with data
        When(Method(USART_mock, read_char)).Return(len , GROUP,   ID, 0x2F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
                                                   0xFF,  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,          
                                                   0xFF,  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
                                                   0xFF,  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
                                                   0xFF,  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, '\n');
        Fake(Method(USART_mock, write_char));

        // Mock all test function to echo the input string data
        When(Method(Commands_mock, CmdTest)).Do([](uint8_t n, uint8_t *pInBuf, cli_rsp *pOutRsp)->cli_status
        {
            // First byte is total number of data bytes
            pOutRsp->length = pInBuf[0];

            // Copy input to output
            for (uint8_t i=0; i<pOutRsp->length; i++)
            {
                pOutRsp->data[i] = pInBuf[i];
            }

            return CLI_STATUS_SUCCESS;
        });

        // Test complete receive + decode + transmit
        status = cli_transceive(inBuf, outBuf);

        // Check that return status is OK
        REQUIRE(status == CLI_STATUS_SUCCESS);

        // Check that "Test<ID>" was called with right parameters
        Verify(Method(Commands_mock, CmdTest).Using((uint8_t) ID, &((cli_in *)inBuf)->command.data[0], &((cli_out *)outBuf)->response)).Exactly(1);

        // Check that only length + status + data was trasnmitted
        Verify(Method(USART_mock,write_char)).Exactly(49);

        // Check that length + status + data was transmitted in the right order
        Verify(Method(USART_mock,write_char).Using(((cli_out *)outBuf)->response.length +2) + \
               Method(USART_mock,write_char).Using(((cli_out *)outBuf)->status)             + \
               Method(USART_mock,write_char).Using(0x2F)                                       + \
               Method(USART_mock,write_char).Using(0xFF) * 0x2E).Exactly(1);

        // Check response and status returned
        REQUIRE(((cli_out *)outBuf)->response.length  == 0x2F);
        REQUIRE(((cli_out *)outBuf)->status == CLI_STATUS_SUCCESS);
    }

    SECTION("Error - Command not exist")
    {
        // Test all command in the CmdTable
        uint8_t GROUP = 0x01;
        auto    ID    = GENERATE(0,1,2,3,4,5,6,7,8,9);

        uint8_t len  = 3;
        
        When(Method(USART_mock, read_char)).Return(len , GROUP,   ID);
        Fake(Method(USART_mock, write_char));
        Fake(Method(Commands_mock, CmdTest));

        // Test complete receive + decode + transmit
        status = cli_transceive(inBuf, outBuf);

        // Check that return status is error
        REQUIRE(                        status == CLI_STATUS_ERROR_CMD_NOT_EXIST);
        REQUIRE(((cli_out *)outBuf)->status == CLI_STATUS_ERROR_CMD_NOT_EXIST);


        // Check response length is 0
        REQUIRE(((cli_out *)outBuf)->response.length == 0);

        // Check that "Test<ID>" was called with right parameters
        VerifyNoOtherInvocations(Method(Commands_mock, CmdTest));

        // Check that length + status was transmitted in the right order
        Verify(Method(USART_mock,write_char).Using(CLI_OUTPUT_HEADER_SIZE) + \
               Method(USART_mock,write_char).Using(((cli_out *)outBuf)->status)).Exactly(1);
    }

    SECTION("Error - Wrong header received")
    {
        // Test all command in the CmdTable
        uint8_t GROUP = GENERATE(0,1,2,3,4,5,6,7,8,9);
        uint8_t len   = GENERATE(0,1,2);
        
        When(Method(USART_mock, read_char)).Return(len , GROUP);
        Fake(Method(USART_mock, write_char));
        Fake(Method(Commands_mock, CmdTest));

        // Test complete receive + decode + transmit
        status = cli_transceive(inBuf, outBuf);

        // Check that return status is error
        REQUIRE(                        status == CLI_STATUS_ERROR_WRONG_HEADER);
        REQUIRE(((cli_out *)outBuf)->status == CLI_STATUS_ERROR_WRONG_HEADER);

        // Check response length is 0
        REQUIRE(((cli_out *)outBuf)->response.length == 0);

        // Check that no "Test<ID>" was called
        VerifyNoOtherInvocations(Method(Commands_mock, CmdTest));

        // Check that length + status was transmitted in the right order
        Verify(Method(USART_mock,write_char).Using(CLI_OUTPUT_HEADER_SIZE) + \
               Method(USART_mock,write_char).Using(((cli_out *)outBuf)->status)).Exactly(1);
    }
}