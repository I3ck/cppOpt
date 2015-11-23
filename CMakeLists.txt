project(cppOpt)

cmake_minimum_required(VERSION 2.8)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

set(CMAKE_CXX_FLAGS "-std=c++11 -Wall -Wextra")
set(CMAKE_EXE_LINKER_FLAGS "-lpthread")

include_directories(
        inc/
)

aux_source_directory(
        src/
        SRC_LIST
)


add_executable(
        example_simulated_annealing_x_square
        examples/example_simulated_annealing_x_square.cpp
)

add_executable(
        example_simulated_annealing_x_square_threaded
        examples/example_simulated_annealing_x_square_threaded.cpp
)

add_executable(
        example_simulated_annealing_rastrigrin_one_dimension
        examples/example_simulated_annealing_rastrigrin_one_dimension.cpp
)

add_executable(
        example_simulated_annealing_rastrigrin_two_dimensions
        examples/example_simulated_annealing_rastrigrin_two_dimensions.cpp
)

add_executable(
        example_great_deluge_x_square
        examples/example_great_deluge_x_square.cpp
)

add_executable(
        example_evolutionary_x_square
        examples/example_evolutionary_x_square.cpp
)

add_executable(
        example_threshold_accepting
        examples/example_threshold_accepting.cpp
)

add_executable(
        example_boundary_split
        examples/example_boundary_split.cpp
)

add_executable(
        test_1
        tests/test_1.cpp
)
