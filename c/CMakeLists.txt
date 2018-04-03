cmake_minimum_required(VERSION 3.9)
project(systat)

set(CMAKE_CXX_STANDARD 11)

add_executable(systat-linux
        common/Console.cpp
        common/Console.h
        common/Log.cpp
        common/Log.h
        common/Parser.cpp
        common/Parser.h
        linux/main.cpp
        linux/Memory.cpp
        linux/Memory.h
        linux/Platform.cpp
        linux/Platform.h
        linux/Processor.cpp
        linux/Processor.h
        linux/Disk.cpp
        linux/Disk.h
        linux/VirtualMemory.cpp
        linux/VirtualMemory.h
        linux/Network.cpp
        linux/Network.h
        linux/systat.h
        )
add_executable(systat-mac
        common/Console.cpp
        common/Console.h
        common/Log.cpp
        common/Log.h
        mac/main.cpp
        mac/Memory.cpp
        mac/Memory.h
        mac/Network.cpp
        mac/Network.h
        mac/Platform.cpp
        mac/Platform.h
        mac/Processor.cpp
        mac/Processor.h
        mac/systat.h mac/Disk.cpp mac/Disk.h mac/VirtualMemory.cpp mac/VirtualMemory.h)

find_library(carbon_lib Carbon) # we look for the Carbon framework and use carbon_lib as an alias for it
find_library(iokit_lib IOKit)
find_library(forcefeedback_lib ForceFeedback)
find_library(cocoa_lib Cocoa)
find_library(audiounit_lib AudioUnit)
find_library(coreaudio_lib CoreAudio)
find_library(opengl_lib OpenGL)
find_library(corefoundation_lib CoreFoundation)
set(frameworks
        ${carbon_lib}
        ${iokit_lib}
        ${forcefeedback_lib}
        ${cocoa_lib}
        ${audiounit_lib}
        ${coreaudio_lib}
        ${opengl_lib}
        ${corefoundation_lib})

target_link_libraries(systat-mac ${frameworks})
