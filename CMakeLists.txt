cmake_minimum_required(VERSION 3.17)
project(ppmanipulation C)

set(CMAKE_C_STANDARD 11)

include_directories(.)

add_executable(transform
        src/transform.c)

add_executable(colour
        src/colour.c)

target_link_libraries(colour m)

add_executable(recut
        src/recut.c)