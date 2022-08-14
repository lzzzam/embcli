import serial
import logging
from . import protocol
from . import report

# Serial interface object
s = None

def initSerial(port, baudrate):
    global s
    logging.info(f"Initializa serial PORT={port}, BAUDRATE={baudrate}")
    s = serial.Serial(port, baudrate)
    
def closeSerial():
    global s
    logging.info(f"Closing serial communication")
    s.close()
    
def sendCommand(cmd):
    global s
    report.logCommand(cmd)
    s.write(cmd.serialize())
    
def receiveResponse():
    global s
    
    buf = b''
    # Read frame length as first byte
    buf += s.read(1)
    # Read remaining part of the frame
    buf += s.read(buf[0]-1)
    
    report.logResponse(buf)
    return protocol.Response(buf)

def executeCommand(cmd):
    logging.info(f"{cmd}")
    sendCommand(cmd)
    rsp = receiveResponse()
    logging.info(f"{rsp}")
    return rsp