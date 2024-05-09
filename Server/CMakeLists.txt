cmake_minimum_required(VERSION 3.16)
project(gardn-server)

include_directories(..)
include_directories(../Shared)

set(SOURCES
    Main.cc
    ../Shared/Entity.cc
    ../Shared/Binary.cc
    ../Shared/Vector.cc
)

if(DEBUG)
    set(CMAKE_CXX_FLAGS "-std=c++17 -gdwarf-4 -lpthread")
else()
    set(CMAKE_CXX_FLAGS "-std=c++17 -O3")
endif()
set(CMAKE_CXX_COMPILER "clang++")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DSERVER=1")

add_executable(gardn-server ${SOURCES})
if(WINDOWS)
    target_link_libraries(gardn-server ws2_32 wsock32)
endif()