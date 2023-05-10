cmake_minimum_required(VERSION 3.24)
project(UnderwaterLife C)

set(CMAKE_C_STANDARD 11)


find_package(C:/SFML-2.5.1/include/)

add_executable(UnderwaterLife main.c)
target_link_libraries(UnderwaterLife csfml-graphics csfml-audio csfml-system)
