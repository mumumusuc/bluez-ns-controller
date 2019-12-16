cmake_minimum_required(VERSION 3.0)
project(controller)

#set(CMAKE_CXX_STANDARD 11)
#SET(CMAKE_CXX_COMPILER g++)

aux_source_directory(${CMAKE_SOURCE_DIR} src)
link_libraries("pthread")
add_compile_options("-fPIC")
add_library(${PROJECT_NAME} SHARED ${src})
add_executable(main ${src})
