import unittest
import logging
from host.report import initLog, startTestLog, stopTestLog
from host.protocol import Command, CLI_STATUS_SUCCESS
from host.interface import initSerial, closeSerial, executeCommand
from host.structs import Buffer

SERIAL_PORT = "/dev/tty.SLAB_USBtoUART"

TestBuf    = Buffer(length=4, data=[1,2,3,4])
ECHO       = Command(group=0, id=0, data=TestBuf)
LED_ON     = Command(group=0, id=1)
LED_OFF    = Command(group=0, id=2)

class Test_Library(unittest.TestCase):
    
    # Executed once at beginning of all tests
    def setUpClass():
        initLog("./log/test_embcli.log")
        try:
            initSerial(SERIAL_PORT, 9600)
        except:
            raise Exception(f"Opening {SERIAL_PORT} FAIL!!")
        
    # Executed once at end of all tests
    def tearDownClass():
        closeSerial()

    # Executed before each test
    def setUp(self):
        startTestLog(self)
    
    # Executed after each test
    def tearDown(self):
        stopTestLog(self)
    
    def test_ECHO(self):
        rsp = executeCommand(ECHO)
        self.assertEqual(rsp.status         , CLI_STATUS_SUCCESS)
        self.assertEqual(Buffer(rsp.data)   , TestBuf)

    def test_LED_ON(self):
        rsp = executeCommand(LED_ON)
        self.assertEqual(rsp.status, CLI_STATUS_SUCCESS)

    def test_LED_OFF(self):
        rsp = executeCommand(LED_OFF)
        self.assertEqual(rsp.status, CLI_STATUS_SUCCESS)


if __name__ == '__main__':
    unittest.main()