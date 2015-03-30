project(cppOpt)

cmake_minimum_required(VERSION 2.8)

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
        src/main.cpp
)

target_link_libraries(example_1 cppOpt)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -pthread")
