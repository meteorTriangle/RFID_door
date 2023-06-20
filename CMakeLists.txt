cmake_minimum_required(VERSION 3.13)

set(PICO_SDK_PATH "${PICO_SDK_PATH}")
set(PICO_BOARD pico CACHE STRING "Board type")

project(rfid_door C CXX ASM)

add_subdirectory(pico_code)