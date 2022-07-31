.SECONDARY:
ROOT   = /Users/luca/Documents/GitHub/Embedded-Software/Tools
CC_DIR = $(ROOT)/gcc-arm-none-eabi-10.3-2021.10/bin
CC     = $(CC_DIR)/arm-none-eabi-gcc
GDB    = $(CC_DIR)/arm-none-eabi-gdb
AR     = $(CC_DIR)/arm-none-eabi-ar
SIZE   = $(CC_DIR)/arm-none-eabi-size
OBJCOPY = $(CC_DIR)/arm-none-eabi-objcopy
OBJDUMP = $(CC_DIR)/arm-none-eabi-objdump
READELF = $(CC_DIR)/arm-none-eabi-readelf

CFLAGS += -mcpu=cortex-m4 -mthumb -Og -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -ffreestanding -flto \
		  -Wunused -Wuninitialized -Wall -Wextra -Wmissing-declarations -Wconversion -Wpointer-arith -Wshadow -Wlogical-op \
		  -Waggregate-return -Wfloat-equal -Wmissing-prototypes -Wstrict-prototypes -Wbad-function-cast -g3


CFLAGS += $(foreach file,$(INC),-I"$(file)")

# Linker options
LFLAGS = -L"./test/systest/onChip/ldscript" -T mem.ld -nostartfiles -Xlinker --gc-sections -Wl,-Map=$(OUTPUT).map

# Link Time Optimization plugin used for library generation
PLUGIN    = $(ROOT)/gcc-arm-none-eabi-10.3-2021.10/lib/gcc/arm-none-eabi/10.3.1/liblto_plugin.0.so

# Define here your main target
TARGET  = test/systest/onChip/main.c

# Startup file to boot-up system
STARTUP = test/systest/onChip/device/startup.c

# Main output directory
BUILD_DIR = ./bin

# Include paths
INC = \
./inc \
./test/systest/onChip/device/inc

# Output Library name
LIB 	  = embcli

# Source files to compile for the library
LIB_SRC   += \
src/embcli.c 

# Source files to compile for the demo
DEMO_SRC += \
$(STARTUP) \
test/systest/onChip/main.c \
test/systest/onChip/device/system.c \
test/systest/onChip/device/RCC.c \
test/systest/onChip/device/GPIO.c \
test/systest/onChip/device/USART.c 


OUTPUT    = $(foreach file, $(TARGET),$(BUILD_DIR)/$(file:%.c=%))
OBJS 	  = $(foreach file, $(DEMO_SRC),$(BUILD_DIR)/$(file:%.c=%.o))
LIB_OBJS  = $(foreach file, $(LIB_SRC),$(BUILD_DIR)/$(file:%.c=%.o))

all: $(BUILD_DIR) $(OUTPUT).elf $(OUTPUT).hex

%.hex: %.elf
	$(OBJCOPY) -O ihex $^ $@

%.elf: %.o $(OBJS) $(LIB).a
	@echo 'Building binary: $<'
	@echo 'Invoking: GNU ARM Cross C Compiler'
	$(CC) $(OBJS) $(CFLAGS) $(LFLAGS) -L"./bin" -l$(LIB) -o $@

$(BUILD_DIR)/%.o: %.c 
	@echo 'Building file: $<'
	@echo 'Invoking: GNU ARM Cross C Compiler'
	$(CC) $(CFLAGS) -c "$<" -o "$@" 
	@echo 'Finished building: $<'
	@echo ' '

$(LIB).a: $(LIB_OBJS)
	@echo 'Generating library: $@'
	$(AR) -rc $(BUILD_DIR)/lib$(LIB).a $^ --plugin $(PLUGIN) 

$(BUILD_DIR):
	@echo 'Creating build directories'
	@mkdir -p $(BUILD_DIR)/src
	@mkdir -p $(BUILD_DIR)/test
	@mkdir -p $(BUILD_DIR)/test/systest
	@mkdir -p $(BUILD_DIR)/test/systest/onChip
	@mkdir -p $(BUILD_DIR)/test/systest/onChip/device
	@mkdir -p $(BUILD_DIR)/test/unittest
	@echo '$(BUILD_DIR) created'

.PHONY: clean
clean:
	rm -r $(BUILD_DIR)

.PHONY: size
size: $(OUTPUT).elf
	$(SIZE) $^

.PHONY: symbol
symbol: $(OUTPUT).elf
	$(READELF) -s $^

.PHONY: sections
sections: $(OUTPUT).elf
	$(READELF) -S $^

.PHONY: disassembly
disassembly: $(OUTPUT).elf
	@echo 'Generate Disassemby: $(OUTPUT).s'
	@echo 'Invoking: arm-none-eabi-objdump'
	$(OBJDUMP) -d -S $(OUTPUT).elf > $(OUTPUT).s

.PHONY: setup
setup:
	@echo 'Setup python environment'
	@echo 'Invoking: pip'
	pip install -r requirements.txt

openocd:
	openocd -f interface/jlink.cfg -c "transport select swd" -f target/stm32f3x.cfg

debug:
	arm-none-eabi-gdb $(OUTPUT).elf -ex "target remote tcp::3333"

.PHONY: foo
foo: $(OBJS)
	@echo $^
	@echo $@

include test/unittest/makefile_ut.mk