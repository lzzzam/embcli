import serial
import cstruct
from protocol import Command, CommandFrame, ResponseFrame

class Position(cstruct.MemCStruct):
    __byte_order__ = cstruct.LITTLE_ENDIAN
    __def__ = """
        struct {
            uint8_t pos_x;
            uint8_t pos_y;
        }
    """
    
# Serial interface object
s = None

def initSerial(port, baudrate):
    s = serial.Serial(port, baudrate)  
  
def sendCommand(cmd):
    frame = CommandFrame(cmd=cmd)
    s.write(frame.pack())
    
def receiveResponse():
    buf = b''
    # Read frame length as first byte
    buf += s.read(1)
    # Read remaining part of the frame
    buf += s.read(buf[0]-1)
    
    return ResponseFrame(buf)


pos  = Position(pos_x=14, pos_y=15)
cmd  = Command(group=0, id=1, data=pos)
rsp  = ResponseFrame(b'\x04\x00\xAA\xBB')
posResp = Position(rsp.data)

print(cmd)
print(cmd.pack())

print(rsp)
print(posResp)
