cmake_minimum_required(VERSION 3.16)
project(triac_uart_cli LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(triac_uart_cli
    src/triac_uart_cli.cpp
)

target_compile_options(triac_uart_cli PRIVATE -Wall -Wextra -Wpedantic)
