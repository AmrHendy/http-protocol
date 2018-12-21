cmake_minimum_required(VERSION 3.12)
project(networkclient)

set(CMAKE_CXX_STANDARD 14)

add_executable(networkclient main.cpp src/client.cpp src/client.h src/sender.cpp src/sender.h src/receiver.cpp src/receiver.h src/utils.cpp src/utils.h)