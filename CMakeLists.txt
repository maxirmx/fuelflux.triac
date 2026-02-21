cmake_minimum_required(VERSION 3.16)
project(relay LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(PkgConfig REQUIRED)
pkg_check_modules(GPIOD REQUIRED libgpiod)

add_executable(relay main.cpp)
target_include_directories(relay PRIVATE ${GPIOD_INCLUDE_DIRS})
target_link_libraries(relay PRIVATE ${GPIOD_LIBRARIES})
target_compile_options(relay PRIVATE ${GPIOD_CFLAGS_OTHER})
