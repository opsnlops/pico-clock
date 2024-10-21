cmake_minimum_required(VERSION 3.25)

set(FREERTOS_HEAP "4" CACHE STRING "" FORCE)
set(FREERTOS_PORT "TEMPLATE" CACHE STRING "" FORCE)


# Pull in the Pico SDK
include($ENV{PICO_SDK_PATH}/external/pico_sdk_import.cmake)

# Make sure we have the right version of the SDK
if (PICO_SDK_VERSION_STRING VERSION_LESS "2.0.0")
    message(FATAL_ERROR "Raspberry Pi Pico SDK version 2.0.0 (or later) required. Your version is ${PICO_SDK_VERSION_STRING}")
endif()


#set(PICO_PLATFORM rp2350)
if (NOT PICO_PLATFORM AND NOT DEFINED ENV{PICO_PLATFORM})
    message("PICO_PLATFORM not defined, defaulting to rp2040")
    set(PICO_PLATFORM rp2040)
endif()


set(PICO_BOARD pico_w)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

# Pull in FreeRTOS from RPI directly so we get the RP2350 support
if(PICO_PLATFORM STREQUAL "rp2040")
    include($ENV{PICO_SDK_PATH}/../FreeRTOS-Kernel/portable/ThirdParty/GCC/RP2040/FreeRTOS_Kernel_import.cmake)
else()
    if (PICO_PLATFORM STREQUAL "rp2350-riscv")
        include($ENV{PICO_SDK_PATH}/../FreeRTOS-Kernel/portable/ThirdParty/GCC/RP2350_RISC-V/FreeRTOS_Kernel_import.cmake)
    else()
        include($ENV{PICO_SDK_PATH}/../FreeRTOS-Kernel/portable/ThirdParty/GCC/RP2350_ARM_NTZ/FreeRTOS_Kernel_import.cmake)
    endif()
endif()


# Pull in the Pico SDK
include($ENV{PICO_SDK_PATH}/external/pico_sdk_import.cmake)

# Make sure we have the right version of the SDK
if (PICO_SDK_VERSION_STRING VERSION_LESS "2.0.0")
    message(FATAL_ERROR "Raspberry Pi Pico SDK version 2.0.0 (or later) required. Your version is ${PICO_SDK_VERSION_STRING}")
endif()


project(pico-clock
        VERSION "0.0.1"
        DESCRIPTION "Pi Pico Clock"
        HOMEPAGE_URL https://github.com/opsnlops/
        LANGUAGES C CXX ASM)

# Initialize the SDK
pico_sdk_init()

# Ensure we're using the hardware floating point
set(PICO_DEFAULT_FLOAT_IMPL pico_float_pico)
set(PICO_DEFAULT_DOUBLE_IMPL pico_double_pico)



set(COMMON_INCLUDES
        src/
        generated/
        ${CMAKE_BINARY_DIR}/generated/
)


add_executable(pico-clock
        src/main.c
        src/lwipopts.h
        src/wifi.h
        src/wifi.c
        src/types.h
        src/wifi_creds.h
        src/sntp.c
        src/sntp.h
        FreeRTOSConfig.h
        src/freertos_hooks.c
        src/logging/logging.c
        src/logging/logging.h
        src/logging/logging_api.h
        src/application-config.h
)


target_include_directories(pico-clock PRIVATE
        ${COMMON_INCLUDES}
        ${CMAKE_CURRENT_LIST_DIR}
)

target_link_libraries(pico-clock
        hardware_rtc
        pico_lwip_sntp
        pico_multicore
        pico_stdlib
        pico_cyw43_arch_lwip_sys_freertos
        pico_async_context_freertos
        FreeRTOS-Kernel-Heap4
)


pico_enable_stdio_usb(pico-clock 0)
pico_enable_stdio_uart(pico-clock 1)
pico_add_extra_outputs(pico-clock)
