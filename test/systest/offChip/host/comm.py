from . import format
import serial

# Serial interface object
s = None

def initSerial(port, baudrate):
    global s
    s = serial.Serial(port, baudrate)
    
def closeSerial():
    global s
    s.close()
    
def sendCommand(cmd):
    global s
    s.write(cmd.serialize())
    
def receiveResponse():
    global s
    
    buf = b''
    # Read frame length as first byte
    buf += s.read(1)
    # Read remaining part of the frame
    buf += s.read(buf[0]-1)
    
    return format.Response(buf)

def executeCommand(cmd):
    sendCommand(cmd)
    rsp = receiveResponse()
    return rsp