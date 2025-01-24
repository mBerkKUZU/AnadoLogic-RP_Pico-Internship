# Set minimum CMake version
cmake_minimum_required(VERSION 3.13)

# Set the project name
project(UART_LED_Controller C CXX ASM)

# Enable the Raspberry Pi Pico SDK
include($ENV{PICO_SDK_PATH}/external/pico_sdk_import.cmake)

# Initialize the Raspberry Pi Pico SDK
pico_sdk_init()

# Add the source file
add_executable(UART_LED_Controller
    project.c  # Replace with the actual file name if different
)

# Specify the target link libraries
target_link_libraries(UART_LED_Controller
    pico_stdlib
    hardware_uart
    hardware_spi
    hardware_i2c
)

# Enable USB output, UART output, and the hardware floating point
pico_enable_stdio_usb(UART_LED_Controller 1)
pico_enable_stdio_uart(UART_LED_Controller 1)

# Add the executable to the output directory
pico_add_extra_outputs(UART_LED_Controller)
