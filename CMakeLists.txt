cmake_minimum_required(VERSION 3.16)

if(DEFINED CMAKE_TOOLCHAIN_FILE)
    message(STATUS "Using toolchain: ${CMAKE_TOOLCHAIN_FILE}")
endif()

project(Stack LANGUAGES C ASM)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS ON)

set(MCU_DEVICE "STM32G431xx")
set(LINKER_SCRIPT "${CMAKE_SOURCE_DIR}/core/STM32G431KBTX_FLASH.ld")

set(ARCH "-mcpu=cortex-m4 -mthumb -mfpu=fpv4-sp-d16 -mfloat-abi=hard")
set(OPT "-fdata-sections -ffunction-sections -Wall -Wextra -Wno-unused-parameter -Wshadow")

set(CMAKE_C_FLAGS "${ARCH} ${OPT} -std=gnu11" CACHE STRING "" FORCE)
set(CMAKE_C_FLAGS_DEBUG "-g3 -Og" CACHE STRING "" FORCE)
set(CMAKE_C_FLAGS_RELEASE "-Os" CACHE STRING "" FORCE)
set(CMAKE_ASM_FLAGS "${ARCH} -x assembler-with-cpp" CACHE STRING "" FORCE)
set(CMAKE_EXE_LINKER_FLAGS "${ARCH} -Wl,--gc-sections -Wl,--print-memory-usage -Wl,-Map=${CMAKE_BINARY_DIR}/${PROJECT_NAME}.map -T${LINKER_SCRIPT} -specs=nano.specs -specs=nosys.specs" CACHE STRING "" FORCE)

set(APP app/main.c app/board.c app/game.c app/menu.c app/rectangle.c app/flash_storage.c)
set(CORE core/Src/system_stm32g4xx.c core/Src/stm32g4xx_it.c core/Src/syscalls.c core/Src/sysmem.c core/Startup/bootloader.c)
file(GLOB_RECURSE HAL "drivers/stm32g4xx_hal/Src/*.c")
file(GLOB_RECURSE BSP "drivers/bsp/stm32g4_*.c")
set(ASM core/Startup/startup_stm32g431kbtx.s drivers/bsp/WS2812/WS2812_asm.s)

include_directories(app core/Inc drivers/stm32g4xx_hal/Inc drivers/cmsis/Include drivers/cmsis/Device/ST/STM32G4xx/Include drivers/bsp)

add_executable(${PROJECT_NAME}.elf ${APP} ${CORE} ${ASM} ${HAL} ${BSP})
add_definitions(-D${MCU_DEVICE} -DUSE_HAL_DRIVER -DARM_MATH_CM4)
target_link_libraries(${PROJECT_NAME}.elf m)

add_custom_command(TARGET ${PROJECT_NAME}.elf POST_BUILD
    COMMAND ${CMAKE_OBJCOPY} -Obinary ${CMAKE_BINARY_DIR}/${PROJECT_NAME}.elf ${CMAKE_BINARY_DIR}/${PROJECT_NAME}.bin
    COMMAND ${CMAKE_OBJCOPY} -Oihex ${CMAKE_BINARY_DIR}/${PROJECT_NAME}.elf ${CMAKE_BINARY_DIR}/${PROJECT_NAME}.hex
    COMMAND ${CMAKE_SIZE} ${CMAKE_BINARY_DIR}/${PROJECT_NAME}.elf > /dev/null 2>&1)

add_custom_target(flash DEPENDS ${PROJECT_NAME}.elf COMMAND STM32_Programmer_CLI -c port=SWD -w ${CMAKE_BINARY_DIR}/${PROJECT_NAME}.elf)
add_custom_target(flash_rst DEPENDS ${PROJECT_NAME}.elf COMMAND STM32_Programmer_CLI -c port=SWD -w ${CMAKE_BINARY_DIR}/${PROJECT_NAME}.elf -rst)
add_custom_target(flash_bin DEPENDS ${PROJECT_NAME}.elf COMMAND STM32_Programmer_CLI -c port=SWD -w ${CMAKE_BINARY_DIR}/${PROJECT_NAME}.bin 0x08000000)
add_custom_target(erase COMMAND STM32_Programmer_CLI -c port=SWD -e all)
