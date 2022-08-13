import unittest
from host.format import Command, CLI_STATUS_SUCCESS
from host.comm import initSerial, closeSerial, executeCommand
from host.structs import Buffer

SERIAL_PORT = "/dev/tty.SLAB_USBtoUART"

TestBuf    = Buffer(length=4, data=[1,2,3,4])
ECHO       = Command(group=0, id=0, data=TestBuf)
LED_ON     = Command(group=0, id=1)
LED_OFF    = Command(group=0, id=2)

class Test_Library(unittest.TestCase):
    
    def setUpClass():
        try:
            initSerial(SERIAL_PORT, 9600)
        except:
            raise Exception(f"Opening {SERIAL_PORT} FAIL!!")
            
    def tearDownClass():
        closeSerial()
        
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