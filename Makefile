TARGET = stm32f303-pe15
BUILD_DIR = build
OBJ_DIR = $(BUILD_DIR)/obj
INCLUDE_DIR = include

CC = arm-none-eabi-gcc
OBJCOPY = arm-none-eabi-objcopy
SIZE = arm-none-eabi-size

# Путь к ST-LINK Utility
ST_LINK_CLI = "C:\Program Files (x86)\STMicroelectronics\STM32 ST-LINK Utility\ST-LINK Utility\ST-LINK_CLI.exe"

MCU = cortex-m4
CFLAGS = -mcpu=$(MCU) -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16
CFLAGS += -O0 -g -Wall -std=gnu11 -DSTM32F303xC -I$(INCLUDE_DIR) -I$(INCLUDE_DIR)/m-profile
CFLAGS += -fdata-sections -ffunction-sections

LDFLAGS = $(CFLAGS) -Tlinker/STM32F303VCTx_FLASH.ld
LDFLAGS += -Wl,--gc-sections -Wl,-Map=$(BUILD_DIR)/$(TARGET).map
LDFLAGS += --specs=nosys.specs --specs=nano.specs

OBJS = $(OBJ_DIR)/main.o $(OBJ_DIR)/startup_stm32f303xc.o $(OBJ_DIR)/system_stm32f3xx.o

all: create_dirs $(BUILD_DIR)/$(TARGET).bin $(BUILD_DIR)/$(TARGET).hex

# Создание директорий
create_dirs:
	@if not exist "$(BUILD_DIR)" mkdir "$(BUILD_DIR)"
	@if not exist "$(OBJ_DIR)" mkdir "$(OBJ_DIR)"

$(BUILD_DIR)/$(TARGET).elf: $(OBJS) | create_dirs
	$(CC) $(OBJS) $(LDFLAGS) -o $@
	$(SIZE) $@

$(BUILD_DIR)/$(TARGET).hex: $(BUILD_DIR)/$(TARGET).elf
	$(OBJCOPY) -O ihex $< $@

$(BUILD_DIR)/$(TARGET).bin: $(BUILD_DIR)/$(TARGET).elf
	$(OBJCOPY) -O binary $< $@

$(OBJ_DIR)/main.o: src/main.c | create_dirs
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR)/system_stm32f3xx.o: src/system_stm32f3xx.c | create_dirs
	$(CC) $(CFLAGS) -c $< -o $@
	
$(OBJ_DIR)/startup_stm32f303xc.o: src/startup_stm32f303xc.s | create_dirs
	$(CC) -x assembler-with-cpp $(CFLAGS) -c $< -o $@

clean:
	@if exist "$(BUILD_DIR)" rmdir /S /Q "$(BUILD_DIR)"

# Прошивка HEX файла
flash: $(BUILD_DIR)/$(TARGET).hex
	$(ST_LINK_CLI) -c SWD -P $< -V -Rst

.PHONY: all clean flash create_dirs