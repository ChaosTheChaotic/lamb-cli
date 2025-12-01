cmake_minimum_required(VERSION 3.22.1)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_C_COMPILER_LAUNCHER ccache)
set(CMAKE_CXX_COMPILER_LAUNCHER ccache)

project("lamb-cli")

option(BUILD_OFFLINE "Build the croco library using the local version (use if you want to skip update or have no wifi)" OFF)

include(ExternalProject)

set(CROCO_DIR "${CMAKE_SOURCE_DIR}/vendor/croco")
set(CROCO_BUILD_DIR "${CMAKE_BINARY_DIR}/croco")
set(CROCO_LIB_DIR "${CMAKE_SOURCE_DIR}/lib")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin")

set(INCLUDE_CLI "src/cli/include")
set(INCLUDE_CROCO "src/croco/include")
set(INCLUDE_SEND "src/modes/send/include")
set(INCLUDE_REQUEST "src/modes/request/include")
set(INCLUDE_UTILS "src/utils/include")
set(INCLUDE_KEYS "src/keys/include")

file(GLOB SOURCE_CLI "src/cli/*.c")
file(GLOB HEADER_CLI "${INCLUDE_CLI}/*.h")

file(GLOB SOURCE_CROCO "src/croco/*.c")
file(GLOB HEADER_CROCO "${INCLUDE_CROCO}/*.h")

file(GLOB SOURCE_SEND "src/modes/send/*.c")
file(GLOB HEADER_SEND "${INCLUDE_SEND}/*.h")

file(GLOB SOURCE_REQUEST "src/modes/request/*.c")
file(GLOB HEADER_REQUEST "${INCLUDE_REQUEST}/*.h")

file(GLOB SOURCE_UTILS "src/utils/*.c")
file(GLOB HEADER_UTILS "${INCLUDE_UTILS}/*.h")

file(GLOB SOURCE_KEYS "src/keys/*.c")
file(GLOB HEADER_KEYS "${INCLUDE_KEYS}/*.h")

add_executable(
  lambc
  src/main.c
  ${SOURCE_CLI}
  ${HEADER_CLI}

  ${SOURCE_CROCO}
  ${HEADER_CROCO}

  ${SOURCE_SEND}
  ${HEADER_SEND}

  ${SOURCE_REQUEST}
  ${HEADER_REQUEST}

  ${SOURCE_UTILS}
  ${HEADER_UTILS}

  ${SOURCE_KEYS}
  ${HEADER_KEYS}
)

target_include_directories(
  lambc
  PRIVATE
  ${INCLUDE_CLI}
  ${INCLUDE_CROCO}
  ${INCLUDE_SEND}
  ${INCLUDE_REQUEST}
  ${INCLUDE_UTILS}
  ${INCLUDE_KEYS}
)

set_target_properties(lambc PROPERTIES C_STANDARD 17)

if(CMAKE_COMPILER_IS_GNUCC OR CMAKE_C_COMPILER_ID MATCHES "Clang")
    target_compile_options(lambc PRIVATE -Wall -Wextra -Wpedantic -flto -O3 -Wno-strict-prototypes)
endif()

find_program(GO_EXECUTABLE go REQUIRED)

ExternalProject_Add(croco_external
    GIT_REPOSITORY "https://github.com/ChaosTheChaotic/croco"
    GIT_TAG "master"
    PREFIX "${CROCO_DIR}"
    BUILD_IN_SOURCE TRUE
    CONFIGURE_COMMAND ""
    BUILD_COMMAND ${GO_EXECUTABLE} build -buildmode=c-shared -o ${CROCO_BUILD_DIR}/libcroco.so croco_clib.go
    INSTALL_COMMAND ""
    BUILD_BYPRODUCTS ${CROCO_BUILD_DIR}/libcroco.so
    UPDATE_DISCONNECTED ${BUILD_OFFLINE}
)

# Create lib directory and copy library
add_custom_command(
    OUTPUT ${CROCO_LIB_DIR}/libcroco.so
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CROCO_LIB_DIR}
    COMMAND ${CMAKE_COMMAND} -E copy ${CROCO_BUILD_DIR}/libcroco.so ${CROCO_LIB_DIR}/libcroco.so
    DEPENDS croco_external
)

add_custom_target(croco_lib ALL DEPENDS ${CROCO_LIB_DIR}/libcroco.so)

add_dependencies(lambc croco_lib)
target_link_directories(lambc PRIVATE ${CROCO_LIB_DIR})
target_link_libraries(lambc PRIVATE croco)
