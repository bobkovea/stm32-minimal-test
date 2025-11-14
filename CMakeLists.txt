cmake_minimum_required(VERSION 3.16)

set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR ARM)

set(TOOLCHAIN_PATH "C:/Program Files (x86)/GNU Arm Embedded Toolchain/10 2021.10/bin")

set(CMAKE_C_COMPILER "${TOOLCHAIN_PATH}/arm-none-eabi-gcc.exe")
set(CMAKE_CXX_COMPILER "${TOOLCHAIN_PATH}/arm-none-eabi-g++.exe")
set(CMAKE_ASM_COMPILER "${TOOLCHAIN_PATH}/arm-none-eabi-gcc.exe")
set(CMAKE_OBJCOPY "${TOOLCHAIN_PATH}/arm-none-eabi-objcopy.exe")
set(CMAKE_SIZE "${TOOLCHAIN_PATH}/arm-none-eabi-size.exe")

set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

project(stm32f303-min-test C CXX ASM)

# Флаги компилятора
set(CPU_FLAGS "-mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16")
set(CMAKE_C_FLAGS "${CPU_FLAGS} -O0 -g -std=gnu11 -DSTM32F303xC -fdata-sections -ffunction-sections")
set(CMAKE_CXX_FLAGS "${CPU_FLAGS} -O0 -g -std=gnu++14 -DSTM32F303xC -fdata-sections -ffunction-sections")
set(CMAKE_ASM_FLAGS "${CPU_FLAGS} -x assembler-with-cpp")

include_directories(include)

# Автоматический поиск .c файлов
file(GLOB_RECURSE SOURCES
    "src/*.c"
)

# Автоматический поиск .s файлов (ассемблерные файлы)
file(GLOB_RECURSE ASM_SOURCES
    "src/*.s"
    "src/*.S"
)

# Объединяем все исходные файлы
set(ALL_SOURCES
    ${SOURCES}
    ${ASM_SOURCES}
)

add_executable(${PROJECT_NAME}
    ${ALL_SOURCES}
)

target_link_options(${PROJECT_NAME} PRIVATE
    -T${CMAKE_SOURCE_DIR}/linker/STM32F303VCTx_FLASH.ld
    -Wl,--gc-sections
    -Wl,-Map=${PROJECT_NAME}.map
    --specs=nosys.specs
    --specs=nano.specs
)

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_OBJCOPY} -O binary $<TARGET_FILE:${PROJECT_NAME}> ${PROJECT_NAME}.bin
    COMMAND ${CMAKE_OBJCOPY} -O ihex $<TARGET_FILE:${PROJECT_NAME}> ${PROJECT_NAME}.hex
    COMMAND ${CMAKE_SIZE} $<TARGET_FILE:${PROJECT_NAME}>
)

# Путь к ST-LINK_CLI
set(STLINK_CLI_PATH "C:/Program Files (x86)/STMicroelectronics/STM32 ST-LINK Utility/ST-LINK Utility/ST-LINK_CLI.exe")

# Цель для прошивки HEX файла
add_custom_target(flash
	COMMAND "${STLINK_CLI_PATH}" -c SWD -P ${PROJECT_NAME}.hex -V -Rst
	DEPENDS ${PROJECT_NAME}.hex
	COMMENT "Flashing HEX file via ST-LINK_CLI..."
)

# Сборка и прошивка за одну команду
add_custom_target(build-flash
    COMMAND ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR} --target ${PROJECT_NAME}
    COMMAND ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR} --target flash
    COMMENT "Building and flashing in one command..."
)