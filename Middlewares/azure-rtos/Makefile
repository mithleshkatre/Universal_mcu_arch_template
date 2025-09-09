# ========================================================
# Minimal ThreadX static library build (Cortex-M4, GCC)
# ========================================================

# Toolchain
CC      = arm-none-eabi-gcc
AR      = arm-none-eabi-ar
CFLAGS  = -mcpu=cortex-m4 -mthumb -mfpu=fpv4-sp-d16 -mfloat-abi=hard \
          -O2 -Wall -fdata-sections -ffunction-sections

# ThreadX include paths (only ThreadX headers)
INCLUDES = -Ithreadx/common/inc \
           -Ithreadx/ports/cortex_m4/gnu/inc

# Output directories
OBJ_DIR = Build
LIB_DIR = Output

# Create output directories
$(shell mkdir -p $(OBJ_DIR))
$(shell mkdir -p $(LIB_DIR))

# ThreadX common C sources
TX_COMMON_SRC = $(wildcard threadx/common/src/*.c)

# ThreadX port assembly sources
TX_PORT_ASM = \
  tx_thread_schedule.S \
  tx_timer_interrupt.S \
  tx_thread_context_restore.S \
  tx_thread_context_save.S \
  tx_thread_interrupt_control.S \
  tx_thread_interrupt_disable.S \
  tx_thread_stack_build.S \
  tx_thread_system_return.S \
  tx_misra.S \
  tx_initialize_low_level.S

# Object files
TX_OBJ_C = $(patsubst %.c,$(OBJ_DIR)/%.o,$(notdir $(TX_COMMON_SRC)))
TX_OBJ_S = $(patsubst %.S,$(OBJ_DIR)/%.o,$(TX_PORT_ASM))
TX_OBJ   = $(TX_OBJ_C) $(TX_OBJ_S)

# ========================================================
# Rule to build threadx.a
# ========================================================
$(LIB_DIR)/threadx.a: $(TX_OBJ)
	$(AR) rcs $@ $^

# Compile C sources
$(OBJ_DIR)/%.o: threadx/common/src/%.c
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

# Compile ASM sources (.S)
$(OBJ_DIR)/%.o: threadx/ports/cortex_m4/gnu/src/%.S
	$(CC) $(CFLAGS) -x assembler-with-cpp $(INCLUDES) -c $< -o $@

# ========================================================
# Clean
# ========================================================
clean:
	rm -f $(OBJ_DIR)/*.o
