##########################################################################################################################
# Enhanced Makefile for STM32 all family support
##########################################################################################################################

TARGET = Bootloader
DEBUG = 1
#1.(-O0: ~32) KB binary
#2.(-Og: ~16 KB (half the size, still debuggable))
#3.(-O2: ~12 KB) ((smaller, harder to debug))
#4.(-Os: ~10 KB) binary (smallest, hardest to debug)
OPT = -Og    # default: -Og
BUILD_DIR = Build

# Total memory (adjust if you switch MCU)
FLASH_TOTAL := 65536  # 64 KB
RAM_TOTAL   := 131072  # 128 KB
#######################################
# Select MCU Family (default = F4)
#######################################
MCU_FAMILY ?= F4

ifeq ($(MCU_FAMILY),F4)
CPU         = -mcpu=cortex-m4
FPU         = -mfpu=fpv4-sp-d16
FLOAT_ABI   = -mfloat-abi=hard
C_DEFS      = -DUSE_HAL_DRIVER -DSTM32F446xx
LDSCRIPT    = bl_linker.ld
ASM_SOURCES = ../HAL/ST/stm32f4/STM32F4xx_HAL_Driver/startup_stm32f4.s
endif

ifeq ($(MCU_FAMILY),F7)
CPU         = -mcpu=cortex-m7
FPU         = -mfpu=fpv5-sp-d16
FLOAT_ABI   = -mfloat-abi=hard
C_DEFS      = -DUSE_HAL_DRIVER -DSTM32F767xx
LDSCRIPT    = stm32f767zi_flash.ld
ASM_SOURCES = startup_stm32f767xx.s
endif

ifeq ($(MCU_FAMILY),H7)
CPU         = -mcpu=cortex-m7
FPU         = -mfpu=fpv5-d16
FLOAT_ABI   = -mfloat-abi=hard
C_DEFS      = -DUSE_HAL_DRIVER -DSTM32H743xx
LDSCRIPT    = stm32h743zi_flash.ld
ASM_SOURCES = startup_stm32h743xx.s
endif

#######################################
# Compiler / Tools
#######################################
PREFIX = arm-none-eabi-
ifdef GCC_PATH
CC = $(GCC_PATH)/$(PREFIX)gcc
CXX =$(GCC_PATH)/$(PREFIX)g++
AS = $(GCC_PATH)/$(PREFIX)gcc -x assembler-with-cpp
CP = $(GCC_PATH)/$(PREFIX)objcopy
SZ = $(GCC_PATH)/$(PREFIX)size
else
CC = $(PREFIX)gcc
CXX = $(PREFIX)g++
AS = $(PREFIX)gcc -x assembler-with-cpp
CP = $(PREFIX)objcopy
SZ = $(PREFIX)size
endif
HEX = $(CP) -O ihex
BIN = $(CP) -O binary -S

#######################################
# MCU + Flags
#######################################
MCU = $(CPU) -mthumb $(FPU) $(FLOAT_ABI)
ASFLAGS = $(MCU) $(AS_DEFS) $(AS_INCLUDES) $(OPT) -Wall -fdata-sections -ffunction-sections
CFLAGS += $(MCU) $(C_DEFS) $(C_INCLUDES) $(OPT) -Wall -fdata-sections -ffunction-sections
CXXFLAGS = $(MCU) $(C_DEFS) $(C_INCLUDES) $(OPT) -Wall -fdata-sections -ffunction-sections -fno-exceptions -fno-rtti -std=c++17

ifeq ($(DEBUG),1)
CFLAGS += -g -gdwarf-2
CXXFLAGS += -g -gdwarf-2
endif
CFLAGS += -MMD -MP -MF"$(@:%.o=%.d)"
CXXFLAGS += -MMD -MP -MF"$(@:%.o=%.d)"

#######################################
# Sources
#######################################

C_SOURCES += \
../HAL/ST/stm32f4/Configs/stm32_hal_msp.c \
../App/Src/sysmem.c \
../App/Src/syscalls.c \


ifeq ($(MCU_FAMILY),F4)
C_SOURCES += \
../CMSIS/Vendor_Device/ST/STM32F4xx/Source/system_stm32f4xx.c \
$(wildcard ../HAL/ST/stm32f4/STM32F4xx_HAL_Driver/Src/*.c)
endif

CPP_SOURCES += \
Src/bl_stm32f4xx_it.cpp \
Src/main.cpp \
../HAL/ST/stm32f4/Configs/stm32_clock.cpp \
../HAL/ST/stm32f4/Configs/stm32_hal_init.cpp \
../HAL/ST/stm32f4/Stm32Uart.cpp \
../HAL/ST/stm32f4/Stm32IO.cpp \
../PAL/PALWrappers/Pin.cpp \
../PAL/PALWrappers/UartWrapper.cpp \
../PAL/Board/HardwareInit.cpp \


#######################################
# Includes
#######################################
C_INCLUDES += \
-IInc \
-I../CMSIS/Vendor_Device/ST/STM32F4xx/Include \
-I../CMSIS/Core/Include \
-I../HAL \
-I../HAL/ST/stm32f4/Configs \
-I../HAL/ST/stm32f4 \
-I../PAL \
-I../PAL/PALWrappers \

ifeq ($(MCU_FAMILY),F4)
C_INCLUDES += \
-I../HAL/ST/stm32f4/STM32F4xx_HAL_Driver/Inc \
-I../HAL/ST/stm32f4/STM32F4xx_HAL_Driver/Inc/Legacy
endif

#######################################
# Linker
#######################################
LIBS = -lc -lm -specs=nosys.specs
LDFLAGS = $(MCU) -specs=nano.specs -u _printf_float -T$(LDSCRIPT) $(LIBS) \
          -Wl,-Map=$(BUILD_DIR)/$(TARGET).map,--cref -Wl,--gc-sections

#######################################
# Build
#######################################
# Object files (normalized inside Build/)
OBJECTS := $(patsubst ../%, $(BUILD_DIR)/%, $(C_SOURCES:.c=.o))
OBJECTS += $(patsubst ../%, $(BUILD_DIR)/%, $(CPP_SOURCES:.cpp=.o))
OBJECTS += $(patsubst ../%, $(BUILD_DIR)/%, $(ASM_SOURCES:.s=.o))
OBJECTS += $(patsubst ../%, $(BUILD_DIR)/%, $(ASMM_SOURCES:.S=.o))

#######################################
# Virtual paths
#######################################
vpath %.c $(sort $(dir $(C_SOURCES)))
vpath %.cpp $(sort $(dir $(CPP_SOURCES)))
vpath %.s $(sort $(dir $(ASM_SOURCES)))
vpath %.S $(sort $(dir $(ASMM_SOURCES)))

all: $(BUILD_DIR)/$(TARGET).elf $(BUILD_DIR)/$(TARGET).hex $(BUILD_DIR)/$(TARGET).bin

# Compile C sources
$(BUILD_DIR)/%.o: ../%.c
	@mkdir -p $(dir $@)
	$(CC) -c $(CFLAGS) $< -o $@

$(BUILD_DIR)/%.o: ../%.cpp
	@mkdir -p $(dir $@)
	$(CXX) -c $(CXXFLAGS) $< -o $@

$(BUILD_DIR)/%.o: ../%.s
	@mkdir -p $(dir $@)
	$(AS) -c $(ASFLAGS) $< -o $@

$(BUILD_DIR)/%.o: ../%.S
	@mkdir -p $(dir $@)
	$(AS) -c $(ASFLAGS) $< -o $@

$(BUILD_DIR)/$(TARGET).elf: $(OBJECTS) Makefile
	$(CXX) $(OBJECTS) $(LDFLAGS) -o $@
	@echo "======ELF File Size ========"
	@$(SZ) --format=berkeley $@ | tee $(BUILD_DIR)/$(TARGET).size
	@echo "=================================="
	@echo "======Section Breakdown ========"
	@arm-none-eabi-size -A $(BUILD_DIR)/$(TARGET).elf | \
		grep -E "^(\\.isr_vector|\\.text|\\.rodata|\\.ARM|\\.init_array|\\.fini_array|\\.data|\\.bss|\\._user_heap_stack)" | \
		awk '{printf "%-18s %8d bytes\n", $$1, $$2}'
	@echo "=================================="
	@echo
	@echo "========== Memory Usage =========="
	@awk -v ftotal=$(FLASH_TOTAL) -v rtotal=$(RAM_TOTAL) 'NR==2 { \
		text=$$1; data=$$2; bss=$$3; \
		flash=text+data; ram=data+bss; \
		printf "Flash: %d / %d (%.1f%%)\n", flash, ftotal, (flash*100.0)/ftotal; \
		printf "RAM  : %d / %d (%.1f%%)\n", ram, rtotal, (ram*100.0)/rtotal; \
	}' $(BUILD_DIR)/$(TARGET).size
	@echo "=================================="

$(BUILD_DIR)/%.hex: $(BUILD_DIR)/%.elf | $(BUILD_DIR)
	$(HEX) $< $@

$(BUILD_DIR)/%.bin: $(BUILD_DIR)/%.elf | $(BUILD_DIR)
	$(BIN) $< $@
	@echo "========== BIN Size =========="
	@BIN_SIZE=$$(wc -c < $@); \
	echo "BIN : $$BIN_SIZE bytes ($$((BIN_SIZE/1024)) KB)"
	@echo "=============================="

$(BUILD_DIR):
	@mkdir -p $@

clean:
	-rm -fR $(BUILD_DIR)
