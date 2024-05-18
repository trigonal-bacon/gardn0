set(CMAKE_SYSTEM_NAME Generic)

cmake_minimum_required(VERSION 3.16)

project(gardn-client)
include_directories(..)

set(SRCS
    Assets/Petal.cc
    Assets/Mob.cc
    Render/Renderer.cc
    Render/RenderDrop.cc
    Render/RenderFlower.cc
    Render/RenderHealth.cc
    Render/RenderMob.cc
    Render/RenderPetal.cc
    Ui/InGame/LevelBar.cc
    Ui/InGame/Loadout.cc
    Ui/TitleScreen/Loadout.cc
    Ui/Button.cc
    Ui/Container.cc
    Ui/Element.cc
    Ui/ExternDef.cc
    Ui/Label.cc
    Ui/VisualUtils.cc
    Game.cc
    Input.cc
    Main.cc
    Simulation.cc
    Socket.cc
    ../Shared/Binary.cc
    ../Shared/Entity.cc
    ../Shared/Helpers.cc
    ../Shared/Simulation.cc
    ../Shared/StaticData.cc
    ../Shared/Vector.cc
)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DCLIENT_SIDE=1 -std=c++20")
set(CMAKE_CXX_COMPILER "em++")
if(DEBUG)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DDEBUG=1")
else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -ffast-math --closure=1")
endif()

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -s NO_EXIT_RUNTIME=1 -s EXPORTED_FUNCTIONS=_main,_malloc,_free,_key_event,_mouse_event,_loop,_on_message")

add_executable(gardn-client ${SRCS})

target_link_libraries(gardn-client m)