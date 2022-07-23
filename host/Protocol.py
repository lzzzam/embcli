import cstruct
import serial

cstruct.define("BUF_SIZE", 20)

s = None

class Command(cstruct.MemCStruct):
    __byte_order__ = cstruct.LITTLE_ENDIAN
    __def__ = """
        struct {
            uint8_t group;
            uint8_t id;
            uint8_t _data[BUF_SIZE];
        }
    """

    """
    Constructor modified to initialize "data" with input struct
    """
    def __init__(self, buffer=None, **kargs) -> None:
        group = kargs.get("group", 0)
        id    = kargs.get("id"   , 0)
        # Initialize with inherited constructor method
        super().__init__(group=group, id=id)
        
        # Initialize data buffer with generic input struct
        if "data" in kargs:  
            self.memcpy(2, kargs["data"].__mem__, kargs["data"].size)
    
    @property
    def data(self):
        return self._data
    
    @data.setter
    def data(self, value):
        self.memcpy(2, value.__mem__, value.size)
    
    
class CmdFrame(cstruct.CStruct):
    __byte_order__ = cstruct.LITTLE_ENDIAN
    __def__ = """
        struct {
            uint8_t         length;
            struct Command  cmd;
        }
    """
    
    """
    Constructor modified to initialize "length" automatically
    """
    def __init__(self, buffer=None, **kargs) -> None:
        # Initialize with inherited constructor method
        super().__init__(cmd=kargs["cmd"])
        
        # Initialize length with "onChip_Cmd" size + length field size
        self.length = kargs["cmd"].size + 1
  
def initSerial(port, baudrate):
    s = serial.Serial(port, baudrate)  
  
def sendCommand(cmd):
    str = CmdFrame(cmd=cmd)
    s.write(str.pack())


class SFR(cstruct.MemCStruct):
    __byte_order__ = cstruct.LITTLE_ENDIAN
    __def__ = """
        struct {
            uint32_t address;
            uint32_t mask;
            uint32_t value;
        }
    """

MPU_reg    = SFR(address=0x23456789, mask=0x0000000F, value=0x00000000)
Cmd_GetSFR = Command(group=12, id=5, data=MPU_reg)
Frame      = CmdFrame(cmd=Cmd_GetSFR)

print(Cmd_GetSFR)
print(Frame.pack())