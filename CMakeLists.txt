cmake_minimum_required(VERSION 3.22.1)

project("lamb-cli")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin")

add_executable(lambc src/main.c)
