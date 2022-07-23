MAKEFILE_UT_PATH := $(abspath $(lastword $(MAKEFILE_LIST)))
MAKEFILE_UT_DIR  := $(dir $(MAKEFILE_UT_PATH))

UNITTEST_SRC_FILE += \
$(MAKEFILE_UT_DIR)/embcli_unittest.cpp \
$(MAKEFILE_UT_DIR)/mocks/USART_fake.cpp \
$(MAKEFILE_UT_DIR)/mocks/Commands_fake.cpp \
$(MAKEFILE_UT_DIR)/../../src/embcli.c \

UNITTEST_INC_DIR += \
$(MAKEFILE_UT_DIR) \
$(MAKEFILE_UT_DIR)/catch \
$(MAKEFILE_UT_DIR)/fakeit \
$(MAKEFILE_UT_DIR)/mocks \
$(MAKEFILE_UT_DIR)/header_override \
$(MAKEFILE_UT_DIR)/../../inc \

UNITTEST_SRC = $(foreach file, $(UNITTEST_SRC_FILE),$(realpath $(file)))
UNITTEST_INC = $(foreach dir, $(UNITTEST_INC_DIR),-I$(realpath $(dir)))

# gcc -std=c++11 test.cpp -o test -lstdc++
# g++ -std=c++11 OnChip_UnitTest.cpp mocks/USART_fake.cpp ../onChip/onChip.c -I. -I../onChip -I./catch -I./fakeit -I./header_override -I./mocks -o ./bin/test/unittest/OnChip_UnitTest
unittest: 
	g++ -O0 -std=c++11 -g -D"UNIT_TEST" $(UNITTEST_SRC) $(UNITTEST_INC) -o $(BUILD_DIR)/test/unittest/embcli_unittest -lstdc++