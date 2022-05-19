cmake_minimum_required(VERSION 3.5)
project(CmdPanel)

include_directories(
    include
    "/home/bian/bianLib/"
)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -pthread")

file(GLOB_RECURSE sources 
    "/home/bian/bianLib/time/src/AbsoluteTimer.cpp" 
    "/home/bian/bianLib/multiThread/src/Loop.cpp" 
    "/home/bian/bianLib/CmdPanel/src/CmdPanel.cpp"
    "/home/bian/bianLib/CmdPanel/src/KeyAction.cpp"
)

add_executable(keyboard "example/test_Keyboard.cpp" ${sources} "src/Keyboard.cpp" "src/ContinuousInputCmdPanel.cpp")

add_executable(UnitreeJoystick "example/test_UnitreeJoystick.cpp" ${sources} "src/UnitreeJoystick.cpp" "src/ContinuousInputCmdPanel.cpp")

add_executable(LogitechJoystick "example/test_LogitechJoystick.cpp" ${sources} "src/LogitechJoystick.cpp")

