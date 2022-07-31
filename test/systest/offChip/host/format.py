from xxlimited import Str
import cstruct

# Payloads cmd/rsp buffer size
CMD_BUF_SIZE  = 32
RSP_BUF_SIZE  = 32
cstruct.define("CMD_BUF_SIZE", CMD_BUF_SIZE)
cstruct.define("RSP_BUF_SIZE", RSP_BUF_SIZE)

# String header size in bytes
CLI_COMMAND_HEADER_SIZE  = 3    # length + group + id
CLI_RESPONSE_HEADER_SIZE = 2    # length + status

# CLI status return macros
CLI_STATUS_SUCCESS              = 0
CLI_STATUS_FAIL                 = 1
CLI_STATUS_ERROR_WRONG_HEADER   = 2
CLI_STATUS_ERROR_CMD_NOT_EXIST  = 3
cstruct.typedef("uint8_t", "cli_status")
cstruct.define("CLI_STATUS_SUCCESS",            0)
cstruct.define("CLI_STATUS_FAIL",               1)
cstruct.define("CLI_STATUS_ERROR_WRONG_HEADER", 2)
cstruct.define("CLI_STATUS_ERROR_CMD_NOT_EXIST",3)

class Command(cstruct.MemCStruct):
    __byte_order__ = cstruct.LITTLE_ENDIAN
    __def__ = """
        struct {
            uint8_t length;
            uint8_t group;
            uint8_t id;
            uint8_t _data[CMD_BUF_SIZE];
        }
    """

    """
    Constructor modified to initialize "data" with input struct
    """
    def __init__(self, buffer=None, **kargs) -> None:

        # Initialize with inherited constructor method
        super().__init__(length=CLI_COMMAND_HEADER_SIZE, 
                         group=kargs.get("group", 0), 
                         id=kargs.get("id", 0))
        
        # Initialize data buffer with generic input struct
        if "data" in kargs:
            self.length += kargs["data"].size
            self.memcpy(CLI_COMMAND_HEADER_SIZE, kargs["data"].__mem__, kargs["data"].size)
    
    """
    Read the "_data" private buffer when the code access the pubblic parameter "data"

    Returns:
        byte list: return "_data" buffer as list of byte
    """
    @property
    def data(self):
        return self._data
    
    """
    Update "_data" private buffer with input buffer. 
    The input is extended to fit with the whole "_data" buffer if size is lower than  CMD_BUF_SIZE
    """
    @data.setter
    def data(self, value):
        # Reset data buffer before overwriting with new data
        for i in range(0, CMD_BUF_SIZE):
            self._data[i] = 0
            
        # Copy input to "_data" buffer and update "length"
        self.memcpy(CLI_COMMAND_HEADER_SIZE, value.__mem__, value.size)
        self.length = CLI_COMMAND_HEADER_SIZE + value.size


class Response(cstruct.CStruct):
    __byte_order__ = cstruct.LITTLE_ENDIAN
    __def__ = """
        struct {
            uint8_t     length;
            cli_status  status;
            uint8_t     _data[RSP_BUF_SIZE];
        }
    """
    
    """
    Constructor modified to extend input buffer to fit "_data" buffer size
    """
    def __init__(self, buffer=None, **kargs) -> None:
        if (buffer != None):
            # Extend buffer to full RSP_BUF_SIZE to unpack withour error
            if len(buffer) < self.size:
                diff = self.size - len(buffer)
                
                # Fill with all zeros
                for i in range(0, diff):
                    buffer += b'\x00'
        
        super().__init__(buffer)
    
    """
    Unpack method modified to extend input buffer to fit "_data" buffer size
    and to unpack binary string of every size without throwing an error
    """
    def unpack(self, buffer) -> bool:
        # Extend buffer to full RSP_BUF_SIZE
        if len(buffer) < self.size:
            diff = self.size - len(buffer)
            
            # Fill with all zeros
            for i in range(0, diff):
                buffer += b'\x00'
                
        super().unpack(buffer)

    """
    Return the "_data" buffer as a binary string
    """
    @property
    def data(self):
        return bytes(self._data)


################################################################################################
#                                      TEST CODE                                               #
################################################################################################
#
# Uncomment the following code to test basic features:

# class SFR(cstruct.MemCStruct):
#     __byte_order__ = cstruct.BIG_ENDIAN
#     __def__ = """
#         struct {
#             uint32_t address;
#             uint32_t mask;
#             uint32_t value;
#         }
#     """    
    
# class Buf(cstruct.MemCStruct):
#     __byte_order__ = cstruct.BIG_ENDIAN
#     __def__ = f"""
#         struct {{
#             uint8_t length;
#             uint8_t _data[{CMD_BUF_SIZE-1}];
#         }}
#     """
#     @property
#     def data(self):
#         return self._data
    
#     @data.setter
#     def data(self, value):
#         # Reset data buffer before overwriting with new data
#         for i in range(0, CMD_BUF_SIZE-1):
#             if i < len(value):
#                 self._data[i] = value[i]
#             else:
#                 self._data[i] = 0
    
#     """ Override cstruct __str__ definition
#     """
#     def __str__ (self):
#         str = '['
#         for i in range(0, self.length-1):
#             str += f'{self._data[i]}, '
#         str += f'{self._data[self.length]}]'
        
#         return f'Buf(length={self.length}, data={str})'

# MPU_reg = SFR(address=0x01020304, mask=0x05060708, value=0x090A0B0C)
# Cmd     = Command(group=14, id=15, data=MPU_reg)

# print(Cmd)
# print(Cmd.data)
# print(Cmd.pack())

# Rsp     = Response(b'\x00\x00\x03\x00\x00\x02')
# print(Rsp)
# print(Rsp.length)
# print(Rsp.status)
# print(Rsp.data)

# rcv = Buf()
# rcv.data = b'\x04\xAA\xBB\xCC\xDD'
# print(rcv)
