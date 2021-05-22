cmake_minimum_required(VERSION 3.17)
project(ppm)

set(CMAKE_C_STANDARD 11)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

include_directories(.)

add_executable(transform
        src/transform.c)

add_executable(colour
        src/colour.c)

target_link_libraries(colour m)

add_executable(recut
        src/recut.c)

add_executable(border
        src/border.c)

add_executable(connect
        src/connect.c)

add_executable(combine
        src/combine.c)