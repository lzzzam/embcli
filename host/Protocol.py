import cstruct
import serial

# Payloads cmd/rsp buffer size
CMD_BUF_SIZE  = 32
RSP_BUF_SIZE  = 32
cstruct.define("CMD_BUF_SIZE", CMD_BUF_SIZE)
cstruct.define("RSP_BUF_SIZE", RSP_BUF_SIZE)

# CLI status return macros
cstruct.typedef("uint8_t", "cli_status")
cstruct.define("CLI_STATUS_SUCCESS",            0)
cstruct.define("CLI_STATUS_FAIL",               1)
cstruct.define("CLI_STATUS_ERROR_WRONG_HEADER", 2)
cstruct.define("CLI_STATUS_ERROR_CMD_NOT_EXIST",3)

class Command(cstruct.MemCStruct):
    __byte_order__ = cstruct.LITTLE_ENDIAN
    __def__ = """
        struct {
            uint8_t group;
            uint8_t id;
            uint8_t _data[CMD_BUF_SIZE];
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
    
    
class CommandFrame(cstruct.CStruct):
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
        
    """
    Return command frame as a byte string
    """
    def serialize(self):
        return self.pack()
class ResponseFrame(cstruct.CStruct):
    __byte_order__ = cstruct.LITTLE_ENDIAN
    __def__ = """
        struct {
            uint8_t             length;
            cli_status          status;
            uint8_t             _data[RSP_BUF_SIZE];
        }
    """
    
    def __init__(self, buffer=None, **kargs) -> None:
        
        if (buffer != None):
            # Extend buffer to full RSP_BUF_SIZE to unpack withour error
            if len(buffer) < self.size:
                delta = self.size-len(buffer)
                for i in range(0, delta):
                    buffer += b'\x00'
        
        super().__init__(buffer)
        
    def unpack(self, buffer) -> bool:
        # Extend buffer to full RSP_BUF_SIZE to unpack withour error
        if len(buffer) < self.size:
            delta = self.size-len(buffer)
            for i in range(0, delta):
                buffer += b'\x00'
                
        super().unpack(buffer)

    @property
    def data(self):
        return bytes(self._data)



# class SFR(cstruct.MemCStruct):
#     __byte_order__ = cstruct.BIG_ENDIAN
#     __def__ = """
#         struct {
#             uint32_t address;
#             uint32_t mask;
#             uint32_t value;
#         }
#     """        
    
# class RspData(cstruct.MemCStruct):
#     __byte_order__ = cstruct.LITTLE_ENDIAN
#     __def__ = """
#         struct {
#             uint8_t d0;
#             uint8_t d1;
#             uint8_t d2;
#             uint8_t d3;
#         }
#     """
    


# MPU_reg    = SFR(address=0x23456789, mask=0x0000000F, value=0x00000000)
# Cmd_GetSFR = Command(group=12, id=5, data=MPU_reg)
# Frame      = CommandFrame(cmd=Cmd_GetSFR)

# print(Cmd_GetSFR)
# print(Frame.serialize())

# resp = Response(data=[1,2,3,4,5,6,7,8])
# test = RspData(bytes(resp.data))
# print(resp)
# print(test)

# print(bytes("1234", "utf-8"))
# print(resp.pack())
# print(len(resp.pack()))

# resp2 = Response(data=b'\x08\x01\x02\x03\x04\x05\x06\x07')
# print(resp2)

# rsp = ResponseFrame()
# print(rsp)

# rsp.unpack(b'\x06\x00\x00\x00\x00\xFF\x00\x00\x00\xFF')

# print(RspData(rsp.data))