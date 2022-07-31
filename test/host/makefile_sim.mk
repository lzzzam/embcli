MAKEFILE_SIM_PATH := $(abspath $(lastword $(MAKEFILE_LIST)))
MAKEFILE_SIM_DIR  := $(dir $(MAKEFILE_SIM_PATH))

SIM_SRC_FILE += \
$(MAKEFILE_SIM_DIR)/server.cpp

SIM_INC_DIR += \

SIM_SRC = $(foreach file, $(SIM_SRC_FILE),$(realpath $(file)))
SIM_INC = $(foreach dir, $(SIM_INC_DIR),-I$(realpath $(dir)))

simulation: $(BUILD_DIR)
	g++ -O0 -std=c++11 $(SIM_SRC) $(SIM_INC) -o $(BUILD_DIR)/test/host/server.exe