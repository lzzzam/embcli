import serial
import cstruct

cstruct.define("BUF_SIZE", 20)

class onChip_Payload(cstruct.MemCStruct):
    __byte_order__ = cstruct.LITTLE_ENDIAN
    __def__ = """
        struct {
            uint8_t data[BUF_SIZE];
        }
    """


    

class Position(cstruct.MemCStruct):
    __byte_order__ = cstruct.LITTLE_ENDIAN
    __def__ = """
        struct {
            uint8_t pos_x;
            uint8_t pos_y;
        }
    """

pos  = Position(pos_x=14, pos_y=15)
pos2 = Position(pos_x=255, pos_y=255)
cmd  = onChip_Cmd(group=0, id=1, data=pos)
# str  = onChip_in()

#cmd.data = pos2

print(cmd)
print(cmd.size)
print(cmd.pack())
