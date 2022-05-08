cmake_minimum_required(VERSION 3.5)

project(sdl2-test LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

INCLUDE_DIRECTORIES(src/)
aux_source_directory(src/ SRC_LIST)

#add_executable(sdl2-test main.cpp)
add_executable(${PROJECT_NAME} ${SRC_LIST} "main.cpp")


target_link_libraries(${PROJECT_NAME} GL SDL2 GLEW)
