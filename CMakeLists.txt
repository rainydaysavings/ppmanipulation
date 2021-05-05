cmake_minimum_required(VERSION 3.17)
project(ppmanipulation C)

set(CMAKE_C_STANDARD 11)

include_directories(.)

add_executable(transform
        transform.c)

add_executable(colour
        colour.c)

target_link_libraries(colour m)