project(cppOpt)

cmake_minimum_required(VERSION 2.8)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

set(CMAKE_CXX_FLAGS "-std=c++11")
set(CMAKE_EXE_LINKER_FLAGS "-lpthread")

include_directories(
        inc/
)

aux_source_directory(
        src/
        SRC_LIST
)


add_executable(
        example_1
        examples/example_1.cpp
)

add_executable(
        example_2
        examples/example_2.cpp
)

add_executable(
        example_3
        examples/example_3.cpp
)

add_executable(
        example_4
        examples/example_4.cpp
)

add_executable(
        test_1
        tests/test_1.cpp
)
