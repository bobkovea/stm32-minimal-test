# Project configuration
TARGET = stm32f303-min-test
BUILD_DIR = build
OBJ_DIR = $(BUILD_DIR)/obj

# Tools
CC = arm-none-eabi-gcc
OBJCOPY = arm-none-eabi-objcopy
SIZE = arm-none-eabi-size
ST_LINK_CLI = "C:\Program Files (x86)\STMicroelectronics\STM32 ST-LINK Utility\ST-LINK Utility\ST-LINK_CLI.exe"

# MCU configuration
MCU = cortex-m4
FPU = fpv4-sp-d16

# Compiler flags
CFLAGS = -mcpu=$(MCU) -mthumb -mfloat-abi=hard -mfpu=$(FPU)
CFLAGS += -O0 -ggdb3 -Wall -Wextra -Wpedantic -std=gnu11
CFLAGS += -DSTM32F303xC
CFLAGS += -Iinclude
CFLAGS += -fdata-sections -ffunction-sections
CFLAGS += -ffreestanding -fno-builtin

# Linker flags
LDFLAGS = $(CFLAGS)
LDFLAGS += -Tlinker/STM32F303VCTx_FLASH.ld
LDFLAGS += -Wl,--gc-sections -Wl,-Map=$(BUILD_DIR)/$(TARGET).map
LDFLAGS += -Wl,--print-memory-usage
LDFLAGS += --specs=nosys.specs --specs=nano.specs

# Source files
C_SRCS = $(wildcard src/*.c)
ASM_SRCS = $(wildcard src/*.s)
SRCS = $(C_SRCS) $(ASM_SRCS)

# Object files
OBJS = $(C_SRCS:src/%.c=$(OBJ_DIR)/%.o)
OBJS += $(ASM_SRCS:src/%.s=$(OBJ_DIR)/%.o)

# Default target
all: $(BUILD_DIR)/$(TARGET).hex

# Main targets
$(BUILD_DIR)/$(TARGET).elf: $(OBJS) | $(BUILD_DIR)
	$(CC) $(OBJS) $(LDFLAGS) -o $@
	@echo "=== Memory usage ==="
	$(SIZE) $@
	
$(BUILD_DIR)/$(TARGET).hex: $(BUILD_DIR)/$(TARGET).elf
	$(OBJCOPY) -O ihex $< $@
	@echo "Hex created: $@"

# Compilation rules
$(OBJ_DIR)/%.o: src/%.c | $(OBJ_DIR)
	@echo "Compiling $<"
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR)/%.o: src/%.s | $(OBJ_DIR)
	@echo "Assembling $<"
	$(CC) -x assembler-with-cpp $(CFLAGS) -c $< -o $@

# Directory creation
$(BUILD_DIR) $(OBJ_DIR):
	@mkdir "$@"

# Utilities
clean:
	@if exist "$(BUILD_DIR)" ( \
		echo "Cleaning build directory..." && \
		rmdir /S /Q "$(BUILD_DIR)" && \
		echo "Clean complete" \
	)

flash: $(BUILD_DIR)/$(TARGET).hex
	@echo "Flashing $(TARGET).hex to device..."
	$(ST_LINK_CLI) -c SWD -P $< -V -Rst

# Dependencies
-include $(OBJS:.o=.d)

# Generate dependencies
$(OBJ_DIR)/%.d: src/%.c | $(OBJ_DIR)
	@$(CC) $(CFLAGS) -MM -MT $(@:.d=.o) -MF $@ $<

# Phony targets
.PHONY: all clean flash info