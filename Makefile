##########################################################################################################################
# Enhanced Makefile for STM32 all family support
##########################################################################################################################

TARGET = final
DEBUG = 1
OPT = -Og
BUILD_DIR = Output

#######################################
# Select MCU Family (default = F4)
#######################################
MCU_FAMILY ?= F4

ifeq ($(MCU_FAMILY),F4)
CPU         = -mcpu=cortex-m4
FPU         = -mfpu=fpv4-sp-d16
FLOAT_ABI   = -mfloat-abi=hard
C_DEFS      = -DUSE_HAL_DRIVER -DSTM32F446xx
LDSCRIPT    = Vendor_HAL/ST/Configs/stm32f446retx_flash.ld
ASM_SOURCES = Vendor_HAL/ST/Configs/startup_stm32f446xx.s
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
MCU = $(CPU) -mthumb $(FPU) $(FLOAT-ABI)
ASFLAGS = $(MCU) $(AS_DEFS) $(AS_INCLUDES) $(OPT) -Wall -fdata-sections -ffunction-sections
CFLAGS += $(MCU) $(C_DEFS) $(C_INCLUDES) $(OPT) -Wall -fdata-sections -ffunction-sections
CXXFLAGS = $(MCU) $(C_DEFS) $(C_INCLUDES) $(OPT) -Wall -fdata-sections -ffunction-sections -fno-exceptions -fno-rtti

ifeq ($(DEBUG),1)
CFLAGS += -g -gdwarf-2
CXXFLAGS += -g -gdwarf-2
endif
CFLAGS += -MMD -MP -MF"$(@:%.o=%.d)"
CXXFLAGS += -MMD -MP -MF"$(@:%.o=%.d)"


#######################################
# Sources
#######################################
C_SOURCES = \
App/Src/stm32f4xx_it.c \
Vendor_HAL/ST/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_rcc.c \
Vendor_HAL/ST/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_rcc_ex.c \
Vendor_HAL/ST/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_flash.c \
Vendor_HAL/ST/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_flash_ex.c \
Vendor_HAL/ST/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_flash_ramfunc.c \
Vendor_HAL/ST/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_gpio.c \
Vendor_HAL/ST/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_dma_ex.c \
Vendor_HAL/ST/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_dma.c \
Vendor_HAL/ST/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_pwr.c \
Vendor_HAL/ST/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_pwr_ex.c \
Vendor_HAL/ST/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_cortex.c \
Vendor_HAL/ST/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal.c \
Vendor_HAL/ST/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_exti.c \
Vendor_HAL/ST/Configs/stm32f4xx_hal_msp.c \
CMSIS/Vendor_Device/ST/STM32F4xx/Source/system_stm32f4xx.c \
App/Src/sysmem.c \
APP/Src/syscalls.c 

CPP_SOURCES = \
App/Src/main.cpp \
PAL/Vendor/ST/Src/pal_gpio.cpp \
PAL/Vendor/ST/Src/pal_board.cpp \
PAL/Vendor/ST/Src/pal_clock.cpp

ASMM_SOURCES = 

#######################################
# Includes
#######################################
C_INCLUDES = \
-IApp/Inc \
-IVendor_HAL/ST/STM32F4xx_HAL_Driver/Inc \
-IVendor_HAL/ST/STM32F4xx_HAL_Driver/Inc/Legacy \
-IVendor_HAL/ST/STM32F4xx_HAL_Driver \
-IVendor_HAL/ST/Configs \
-ICMSIS/Vendor_Device/ST/STM32F4xx/Include \
-ICMSIS/Core/Include \
-IPAL/Vendor/common/Inc

#######################################
# Linker
#######################################
LIBS = -lc -lm -lnosys
LIBDIR =
LDFLAGS = $(MCU) -specs=nano.specs -T$(LDSCRIPT) $(LIBDIR) $(LIBS) \
          -Wl,-Map=$(BUILD_DIR)/$(TARGET).map,--cref -Wl,--gc-sections

#######################################
# Build
#######################################
OBJECTS = $(addprefix $(BUILD_DIR)/,$(notdir $(C_SOURCES:.c=.o)))
OBJECTS += $(addprefix $(BUILD_DIR)/,$(notdir $(CPP_SOURCES:.cpp=.o)))
OBJECTS += $(addprefix $(BUILD_DIR)/,$(notdir $(ASM_SOURCES:.s=.o)))
OBJECTS += $(addprefix $(BUILD_DIR)/,$(notdir $(ASMM_SOURCES:.S=.o)))

vpath %.c $(sort $(dir $(C_SOURCES)))
vpath %.cpp $(sort $(dir $(CPP_SOURCES)))
vpath %.s $(sort $(dir $(ASM_SOURCES)))
vpath %.S $(sort $(dir $(ASMM_SOURCES)))

all: $(BUILD_DIR)/$(TARGET).elf $(BUILD_DIR)/$(TARGET).hex $(BUILD_DIR)/$(TARGET).bin

$(BUILD_DIR)/%.o: %.c Makefile | $(BUILD_DIR)
	$(CC) -c $(CFLAGS) $< -o $@
$(BUILD_DIR)/%.o: %.cpp Makefile | $(BUILD_DIR)
	$(CXX) -c $(CXXFLAGS) $< -o $@
$(BUILD_DIR)/%.o: %.s Makefile | $(BUILD_DIR)
	$(AS) -c $(CFLAGS) $< -o $@
$(BUILD_DIR)/%.o: %.S Makefile | $(BUILD_DIR)
	$(AS) -c $(CFLAGS) $< -o $@

$(BUILD_DIR)/$(TARGET).elf: $(OBJECTS) Makefile
	$(CXX) $(OBJECTS) $(LDFLAGS) -o $@
	$(SZ) $@

$(BUILD_DIR)/%.hex: $(BUILD_DIR)/%.elf | $(BUILD_DIR)
	$(HEX) $< $@
$(BUILD_DIR)/%.bin: $(BUILD_DIR)/%.elf | $(BUILD_DIR)
	$(BIN) $< $@
$(BUILD_DIR):
	mkdir $@

clean:
	-rm -fR $(BUILD_DIR)

-include $(wildcard $(BUILD_DIR)/*.d)

$(info ===== Build Configuration =====)
$(info MCU_FAMILY = $(MCU_FAMILY))
$(info CPU        = $(CPU))
$(info FPU        = $(FPU))
$(info FLOAT-ABI  = $(FLOAT-ABI))
$(info C_DEFS     = $(C_DEFS))
$(info LDSCRIPT   = $(LDSCRIPT))
$(info ASM_SOURCES= $(ASM_SOURCES))
$(info ===============================)