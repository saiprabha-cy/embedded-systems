cmake_minimum_required(VERSION 3.16)

project(c10_modular_embedded_architecture C)

set(CMAKE_C_STANDARD 11)

include_directories(include)

add_executable(c10_demo
    src/main.c
    src/sensor.c
    src/hal_i2c.c
)