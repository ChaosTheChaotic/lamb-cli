cmake_minimum_required(VERSION 3.22.1)

project("lamb-cli")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin")

file(GLOB SOURCE_FILES "src/*.c")
file(GLOB HEADER_FILES "src/*.h")

add_executable(lambc ${SOURCE_FILES} ${HEADER_FILES})

target_include_directories(lambc PRIVATE src)
