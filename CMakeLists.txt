cmake_minimum_required(VERSION 3.22)

set(CMAKE_TOOLCHAIN_FILE ${CMAKE_SOURCE_DIR}/cmake/gcc-arm-none-eabi.cmake)
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
# This file is generated only once,
# and is not re-generated if converter is called multiple times.
#
# User is free to modify the file as much as necessary
#

# Setup compiler settings
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS ON)


# Define the build type
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug")
endif()

# Set the project name
set(CMAKE_PROJECT_NAME X1_Panel)

# Enable compile command to ease indexing with e.g. clangd
set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

# Core project settings
project(${CMAKE_PROJECT_NAME} C CXX ASM)
message("Build type: " ${CMAKE_BUILD_TYPE})

# Enable CMake support for ASM and C languages
enable_language(C CXX ASM)

include_directories(
        ${HEADER_FILES}
        ${CMAKE_CURRENT_LIST_DIR}/config
        ${CMAKE_CURRENT_LIST_DIR}/user
        ${CMAKE_CURRENT_LIST_DIR}/user/inc
        ${CMAKE_CURRENT_LIST_DIR}/Arduino
        ${CMAKE_CURRENT_LIST_DIR}/Arduino/avr
        ${CMAKE_CURRENT_LIST_DIR}/Arduino/drivers
        ${CMAKE_CURRENT_LIST_DIR}/Arduino/variants/generic_hc32f460

        ${CMAKE_CURRENT_LIST_DIR}/libraries/
        ${CMAKE_CURRENT_LIST_DIR}/libraries/lwmem/src/include

        ${CMAKE_CURRENT_LIST_DIR}/platform/core
        ${CMAKE_CURRENT_LIST_DIR}/platform/LL_drivers/inc
        ${CMAKE_CURRENT_LIST_DIR}/platform/mcu/cmsis
        ${CMAKE_CURRENT_LIST_DIR}/platform/mcu/common
)

file(GLOB_RECURSE SOURCE_FILE
        ${CMAKE_CURRENT_LIST_DIR}/user/*.c
        ${CMAKE_CURRENT_LIST_DIR}/user/*.cpp
        ${CMAKE_CURRENT_LIST_DIR}/Arduino/*.cpp
        ${CMAKE_CURRENT_LIST_DIR}/Arduino/*.c
        ${CMAKE_CURRENT_LIST_DIR}/platform/*.c
        ${CMAKE_CURRENT_LIST_DIR}/platform/core/*.c
        ${CMAKE_CURRENT_LIST_DIR}/platform/core/*.cpp
        ${CMAKE_CURRENT_LIST_DIR}/platform/LL_drivers/src/*.c
        ${CMAKE_CURRENT_LIST_DIR}/platform/mcu/common/system_hc32f460.c
        ${CMAKE_CURRENT_LIST_DIR}/platform/mcu/syscalls/*.c

        ${CMAKE_CURRENT_LIST_DIR}/libraries/*.c
        ${CMAKE_CURRENT_LIST_DIR}/libraries/*.cpp
    )

SET(STARTUP_FILE ${CMAKE_CURRENT_LIST_DIR}/platform/startup/startup_hc32f460.S)

set(Defines_Syms
        USE_DEVICE_DRIVER_LIB
        HC32F460
        __CORE_DEBUG
)

set(BOOT_MODE 1)
set(FLASH_START 0x00000000)
set(FLASH_END   0x80000)
math(EXPR FLASH_SIZE "${FLASH_END} - ${FLASH_START}")
set(STACK_SIZE 0x00001000)
message("FLASH_SIZE: " ${FLASH_SIZE})

add_custom_command(
        OUTPUT ${CMAKE_CURRENT_LIST_DIR}/platform/startup/linker/hc32f46x_final.ld
        COMMAND arm-none-eabi-gcc -C -P -x c -E
            ${CMAKE_CURRENT_LIST_DIR}/platform/startup/linker/hc32f46x_param.ld
            -o ${CMAKE_CURRENT_LIST_DIR}/platform/startup/linker/hc32f46x_final.ld
            -DLD_FLASH_START=${FLASH_START}
            -DLD_FLASH_SIZE=${FLASH_SIZE}
            -DLD_BOOT_MODE=${BOOT_MODE}
            -DDDL_STACK_SIZE=${STACK_SIZE}
        DEPENDS ${CMAKE_CURRENT_LIST_DIR}/platform/startup/linker/hc32f46x_param.ld
        COMMENT "===Preprocess linker script, generate hc32f46x_final.ld ==="
)
    
add_custom_target(
        gen_linker_script
        DEPENDS ${CMAKE_CURRENT_LIST_DIR}/platform/startup/linker/hc32f46x_final.ld)

add_executable(${CMAKE_PROJECT_NAME} ${SOURCE_FILE} ${STARTUP_FILE})

add_dependencies(${CMAKE_PROJECT_NAME} gen_linker_script)

target_compile_definitions(${CMAKE_PROJECT_NAME} PRIVATE ${Defines_Syms})

message("Link Flag:" ${CMAKE_EXE_LINKER_FLAGS})
# 生成HEX和BIN文件
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/output)
set(HEX_FILE ${EXECUTABLE_OUTPUT_PATH}/${CMAKE_PROJECT_NAME}.hex)
set(BIN_FILE ${EXECUTABLE_OUTPUT_PATH}/${CMAKE_PROJECT_NAME}.bin)

add_custom_command(TARGET ${CMAKE_PROJECT_NAME} POST_BUILD
       COMMAND ${CMAKE_OBJCOPY} -O ihex "${EXECUTABLE_OUTPUT_PATH}/${CMAKE_PROJECT_NAME}.elf" "${HEX_FILE}"
       COMMAND ${CMAKE_OBJCOPY} -O binary "${EXECUTABLE_OUTPUT_PATH}/${CMAKE_PROJECT_NAME}.elf" "${BIN_FILE}"
       COMMENT "Generating HEX and BIN files"
)