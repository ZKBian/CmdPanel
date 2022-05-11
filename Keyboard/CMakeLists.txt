cmake_minimum_required(VERSION 3.12)
project(keyboard)

include_directories(
    include
    "/home/bian/bianLib/"
    "${THIS_COM}"
    # "/home/bian/testField"
    # ${Boost_INCLUDE_DIR}
)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -pthread")

add_executable(keyboard 
    test.cpp 
    src/Keyboard.cpp
    /home/bian/bianLib/time/src/AbsoluteTimer.cpp
    /home/bian/bianLib/multiThread/src/Loop.cpp
    /home/bian/bianLib/CmdPanel/src/CmdPanel.cpp
)