project(cppOpt)

cmake_minimum_required(VERSION 2.8)

include_directories(
        inc/
)

aux_source_directory(
        src/
        SRC_LIST
)

add_executable(
        ${PROJECT_NAME}
        ${SRC_LIST}
)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -pthread")
