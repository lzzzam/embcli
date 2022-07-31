MAKEFILE_QEMU_PATH := $(abspath $(lastword $(MAKEFILE_LIST)))
MAKEFILE_QEMU_DIR  := $(dir $(MAKEFILE_QEMU_PATH))

QEMU_OUTPUT     = test_semihost

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
	arm-none-eabi-gcc $(SEMIHOST_SRC) -I$(SEMIHOST_INC) $(SEMIHOST_CFLAGS) $(SEMIHOST_LFLAGS) -o $(QEMU_OUTPUT).axf