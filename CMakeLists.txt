cmake_minimum_required(VERSION 3.24)
project(UnderwaterLife C)

set(CMAKE_C_STANDARD 99)


find_package(D:/Programming/Libraries/SFML/include/)

add_executable(UnderwaterLife main.c)
target_link_libraries(UnderwaterLife csfml-graphics csfml-audio csfml-system)
