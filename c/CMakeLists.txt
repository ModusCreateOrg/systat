cmake_minimum_required(VERSION 3.9)
project(systat)

set(CMAKE_CXX_STANDARD 11)

add_executable(systat-linux
        common/Console.cpp
        common/Console.h
        linux/main.cpp
        linux/Memory.cpp
        linux/Memory.h
        linux/Parser.cpp
        linux/Parser.h
        linux/Platform.cpp
        linux/Platform.h
        linux/Processor.cpp
        linux/Processor.h
        linux/systat.h
        )
add_executable(systat-mac
        common/Console.cpp
        common/Console.h
        mac/main.cpp
        mac/Memory.cpp
        mac/Memory.h
        mac/Network.cpp
        mac/Network.h
        mac/Platform.cpp
        mac/Platform.h
        mac/Processor.cpp
        mac/Processor.h
        mac/systat.h)
