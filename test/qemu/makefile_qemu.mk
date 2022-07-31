MAKEFILE_QEMU_PATH := $(abspath $(lastword $(MAKEFILE_LIST)))
MAKEFILE_QEMU_DIR  := $(dir $(MAKEFILE_QEMU_PATH))

SEMIHOST_OUTPUT     = test_semihost

SEMIHOST_CFLAGS = -mthumb -mcpu=cortex-m4 -O0 -g3 -flto -ffunction-sections -fdata-sections
SEMIHOST_LFLAGS = --specs=nano.specs --specs=rdimon.specs -Ltest/demo/ldscript -T mem.ld -Wl,--gc-sections -Wl,-Map=$(QEMU_OUTPUT).map

SEMIHOST_SRC_FILE += \
$(STARTUP) \
$(MAKEFILE_QEMU_DIR)/main.c \

SEMIHOST_INC_DIR += \
$(INC)

SEMIHOST_SRC = $(foreach file, $(SEMIHOST_SRC_FILE),$(realpath $(file)))
SEMIHOST_INC = $(foreach dir, $(SEMIHOST_INC_DIR),-I$(realpath $(dir)))

semihost: $(BUILD_DIR)
	$(CC) $(SEMIHOST_SRC) -I$(SEMIHOST_INC) $(SEMIHOST_CFLAGS) $(SEMIHOST_LFLAGS) -o $(BUILD_DIR)/$(SEMIHOST_OUTPUT).axf

qemu: semihost
	$(QEMU) -mcu STM32F407VG --semihosting --image $(BUILD_DIR)/$(SEMIHOST_OUTPUT).axf

qemu_gdb_server:
	$(QEMU) -mcu STM32F407VG --gdb tcp::1234 --semihosting

gdb_client:
	$(GDB) $(BUILD_DIR)/$(SEMIHOST_OUTPUT).axf -ex "target remote tcp::1234" -ex "load" -ex "continue"