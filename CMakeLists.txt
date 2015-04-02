project(cppOpt)

cmake_minimum_required(VERSION 2.8)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -pthread")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static-libgcc -static-libstdc++ -static")

include_directories(
        inc/
)

aux_source_directory(
        src/
        SRC_LIST
)

add_library(
        cppOpt
        STATIC
        src/OptSolverBase.cpp
        src/OptBase.cpp
        src/OptBoundaries.cpp
        src/OptBoundary.cpp
        src/OptSimulatedAnnealing.cpp
        src/OptThresholdAccepting.cpp
        src/OptGreatDeluge.cpp
        src/OptEvolutionary.cpp
        src/OptCalculation.cpp
)

add_executable(
        example_1
        examples/example_1.cpp
)

target_link_libraries(example_1 cppOpt)

add_executable(
        example_2
        examples/example_2.cpp
)

target_link_libraries(example_2 cppOpt)

add_executable(
        test_1
        tests/test_1.cpp
)

target_link_libraries(test_1 cppOpt)
