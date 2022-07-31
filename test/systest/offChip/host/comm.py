from distutils.log import error
import format
import serial

# Serial interface object
s = None

def initSerial(port, baudrate):
    s = serial.Serial(port, baudrate)  

def executeCommand(cmd):
    s.write(cmd.pack())
    data = s.read(format.RSP_BUF_SIZE)
    rsp = format.Response(data)
    
    try:
        assert(rsp.status == format.CLI_STATUS_SUCCESS)
    except:
        error
        
    return rsp