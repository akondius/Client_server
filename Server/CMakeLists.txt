project(Client)
cmake_minimum_required(VERSION 2.6)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++11")
add_definitions(-Wall -O2)
file(GLOB socket_source_files ../*.cpp)
set(SOURCES main.cpp )
include_directories(./)
include_directories(../)
add_executable ( Release/${PROJECT_NAME} ${SOURCES} ${socket_source_files})


