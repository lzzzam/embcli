.SECONDARY:
ROOT   = /Users/luca/Documents/GitHub/Embedded-Software/Toolchain/Compilers
CC_DIR = $(ROOT)/gcc-arm-none-eabi/bin
CC     = $(CC_DIR)/arm-none-eabi-gcc
SIZE   = $(CC_DIR)/arm-none-eabi-size
OBJCOPY = $(CC_DIR)/arm-none-eabi-objcopy
OBJDUMP = $(CC_DIR)/arm-none-eabi-objdump
READELF = $(CC_DIR)/arm-none-eabi-readelf

CFLAGS += -mcpu=cortex-m4 -mthumb -Og -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -ffreestanding -flto \
		  -Wunused -Wuninitialized -Wall -Wextra -Wmissing-declarations -Wconversion -Wpointer-arith -Wshadow -Wlogical-op \
		  -Waggregate-return -Wfloat-equal -Wmissing-prototypes -Wstrict-prototypes -Wbad-function-cast -g3


CFLAGS += $(foreach file,$(INC),-I"$(file)")

# Linker options
LFLAGS = -L"./test/demo/ldscript" -T mem.ld -nostartfiles -Xlinker --gc-sections -Wl,-Map=$(OUTPUT).map

# Define here your main target
TARGET  = test/demo/main.c

# Startup file to boot-up system
STARTUP = test/demo/device/startup.c

# Main output directory
BUILD_DIR = ./bin

# Include paths
INC = \
./onChip \
./test/demo/device/inc

# Source files to compile
SRC += \
$(STARTUP) \
onChip/onChip.c \
onChip/onChip_Cmd.c \
test/demo/device/system.c \
test/demo/device/RCC.c \
test/demo/device/GPIO.c \
test/demo/device/USART.c 


OUTPUT    = $(foreach file, $(TARGET),$(BUILD_DIR)/$(file:%.c=%))
OBJS	  = $(foreach file, $(SRC),$(BUILD_DIR)/$(file:%.c=%.o))

all: $(BUILD_DIR) $(OUTPUT).elf $(OUTPUT).hex

%.hex: %.elf
	$(OBJCOPY) -O ihex $^ $@

%.elf: %.o $(OBJS)
	$(CC) $^ $(CFLAGS) $(LFLAGS) -o $@

$(BUILD_DIR)/%.o: %.c
	@echo 'Building file: $<'
	@echo 'Invoking: GNU ARM Cross C Compiler'
	$(CC) $(CFLAGS) -c "$<" -o "$@" 
	@echo 'Finished building: $<'
	@echo ' '

$(BUILD_DIR):
	@echo 'Creating build directories'
	@mkdir -p $(BUILD_DIR)/onChip
	@mkdir -p $(BUILD_DIR)/test
	@mkdir -p $(BUILD_DIR)/test/demo
	@mkdir -p $(BUILD_DIR)/test/demo/device
	@mkdir -p $(BUILD_DIR)/test/unittest
	@echo '$(BUILD_DIR) created'

.PHONY: clean
clean:
	rm -r $(BUILD_DIR)

.PHONY: size
size: $(OUTPUT).hex
	$(SIZE) $(OUTPUT).hex

.PHONY: symbol
symbol: $(OUTPUT).hex
	$(READELF) -s $^

.PHONY: sections
sections: $(OUTPUT).hex
	$(READELF) -S $^

.PHONY: foo
foo: $(OBJS)
	@echo $^
	@echo $@

include test/unittest/makefile_ut.mk