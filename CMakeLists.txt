cmake_minimum_required(VERSION 3.22.1)

project("lamb-cli")

include(ExternalProject)

set(CROCO_DIR "${CMAKE_SOURCE_DIR}/vendor/croco")
set(CROCO_BUILD_DIR "${CMAKE_BINARY_DIR}/croco")
set(CROCO_LIB_DIR "${CMAKE_SOURCE_DIR}/lib")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin")

file(GLOB SOURCE_CLI "src/cli/*.c")
file(GLOB HEADER_CLI "src/cli/include/*.h")

file(GLOB SOURCE_CROCO "src/croco/*.c")
file(GLOB HEADER_CROCO "src/croco/include/*.h")

add_executable(lambc src/main.c ${SOURCE_CLI} ${HEADER_CLI} ${SOURCE_CROCO} ${HEADER_CROCO})

target_include_directories(lambc PRIVATE src/croco/include src/cli/include)

set_target_properties(lambc PROPERTIES C_STANDARD 17)

if(CMAKE_COMPILER_IS_GNUCC OR CMAKE_C_COMPILER_ID MATCHES "Clang")
    target_compile_options(lambc PRIVATE -Wall -Wextra -Wpedantic -flto -O3)
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
