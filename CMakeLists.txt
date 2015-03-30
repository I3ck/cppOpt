project(cppOpt)

cmake_minimum_required(VERSION 2.8)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -pthread")

include_directories(
        inc/
)

aux_source_directory(
        src/
        SRC_LIST
)

add_library(
        cppOpt
        SHARED
        src/CalculatorBase.cpp
        src/OptBase.cpp
        src/OptBoundaries.cpp
        src/OptBoundary.cpp
        src/OptSimulatedAnnealing.cpp
        src/OptValue.cpp
)

add_executable(
        example_1
        src/example_1.cpp
)

target_link_libraries(example_1 cppOpt)
