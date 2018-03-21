cmake_minimum_required(VERSION 3.9)
project(c)

set(CMAKE_CXX_STANDARD 11)

include_directories(.)

add_executable(c
        Processor.cpp
        main.cpp
        network.cpp
        systat.h Network.h Console.cpp Console.h Processor.h)
