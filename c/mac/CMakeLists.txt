cmake_minimum_required(VERSION 3.9)
project(c)

set(CMAKE_CXX_STANDARD 11)

include_directories(.)

add_executable(systat
        Processor.cpp
        main.cpp
        Network.cpp
        systat.h Network.h Console.cpp Console.h Processor.h Memory.cpp Memory.h Platform.cpp Platform.h)
