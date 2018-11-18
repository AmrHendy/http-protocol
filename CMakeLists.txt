cmake_minimum_required(VERSION 3.12)
project(network2)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -pthread")

add_executable(network2 main.cpp src/server.cpp src/server.h src/receiver.cpp src/receiver.h)