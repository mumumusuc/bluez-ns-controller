cmake_minimum_required(VERSION 3.0)
project(controller C)

set(CMAKE_C_STANDARD 11)
aux_source_directory(${CMAKE_SOURCE_DIR} src)
#add_executable(${PROJECT_NAME} main.c)
#aux_source_directory(${CMAKE_SOURCE_DIR}/ns ns)
#add_executable(${PROJECT_NAME} ${src})
add_library(${PROJECT_NAME} SHARED ${src})