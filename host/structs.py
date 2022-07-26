from xml.etree.ElementInclude import DEFAULT_MAX_INCLUSION_DEPTH
import cstruct
import format

class Reg(cstruct.MemCStruct):
    __byte_order__ = cstruct.BIG_ENDIAN
    __def__ = """
        struct {
            uint32_t address;
            uint32_t value;
        }
    """

