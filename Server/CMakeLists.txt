cmake_minimum_required(VERSION 3.16)
project(gardn-server)

include_directories(..)
include_directories(../Shared)

set(SOURCES
    Main.cc
    Simulation.cc
    Server.cc
    ../Shared/Binary.cc
    ../Shared/Entity.cc
    ../Shared/Helpers.cc
    ../Shared/Simulation.cc
    ../Shared/Vector.cc
)

if(DEBUG)
    set(CMAKE_CXX_FLAGS "-std=c++17 -gdwarf-4 -lpthread")
else()
    set(CMAKE_CXX_FLAGS "-std=c++17 -O3")
endif()
set(CMAKE_CXX_COMPILER "g++")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DSERVER_SIDE=1")

add_executable(gardn-server ${SOURCES})
target_include_directories(gardn-server PRIVATE C:/uWebSockets/src)
target_include_directories(gardn-server PRIVATE C:/uWebSockets/uSockets/src)
target_link_directories(gardn-server PRIVATE C:/uWebSockets/uSockets)
target_link_libraries(gardn-server ws2_32 pthread z m uv uSockets)
