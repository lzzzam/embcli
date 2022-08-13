import cstruct
from . import format

class Reg(cstruct.MemCStruct):
    __byte_order__ = cstruct.BIG_ENDIAN
    __def__ = """
        struct {
            uint32_t address;
            uint32_t value;
        }
    """

class Buffer(cstruct.MemCStruct):
    __byte_order__ = cstruct.LITTLE_ENDIAN
    __def__ = """
        struct {
            uint8_t length;
            uint8_t data[4];
        }
    """
