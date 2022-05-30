#ifndef BIANLIB_LOGITECH_JOYSTICK_H
#define BIANLIB_LOGITECH_JOYSTICK_H

#include "CmdPanel/include/CmdPanel.h"
#include "CmdPanel/include/JoystickState.h"
#include "CmdPanel/include/LogitechJoystickMsg.h"

class LogitechJoystick : public CmdPanel{
public:
    LogitechJoystick(std::string devicePath, 
        std::vector<KeyAction*> events, 
        EmptyAction emptyAction, size_t channelNum = 1,
        double dt = 0.02);
    LogitechJoystick(std::vector<KeyAction*> events, 
        EmptyAction emptyAction, size_t channelNum = 1,
        double dt = 0.02);
    ~LogitechJoystick();
private:
    void _init();
    void _read();
    bool _sample();
    void _extractMsg();
    void _openDevice(std::string devicePath, bool blocking = false);
    void _clearBuffer();
    void _updateJoystick();
    void _updateStickAction();
    void _updatePressState(KeyState &currentState);
    void _checkKeyRepeat(KeyState &currentState);
    void _checkCmdUpdated();

    int _fd;
    double _clickTime;    // s
    LogitechJoystickMsg _msg;
    JoystickState _keyStates;
    size_t _keyNum;
    std::vector<size_t> _combineKeyID;
};

#endif  // BIANLIB_LOGITECH_JOYSTICK_H