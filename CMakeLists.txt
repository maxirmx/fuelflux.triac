cmake_minimum_required(VERSION 3.13)
project(opiz2w_relay_demo LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(PkgConfig REQUIRED)
pkg_check_modules(GPIOD REQUIRED libgpiod)

add_executable(relay_demo main.cpp)
target_include_directories(relay_demo PRIVATE ${GPIOD_INCLUDE_DIRS})
target_link_libraries(relay_demo PRIVATE ${GPIOD_LIBRARIES})
target_compile_options(relay_demo PRIVATE ${GPIOD_CFLAGS_OTHER})
