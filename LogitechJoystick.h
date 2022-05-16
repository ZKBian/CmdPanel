#ifndef BIANLIB_LOGITECH_JOYSTICK_H
#define BIANLIB_LOGITECH_JOYSTICK_H

#include "CmdPanel/include/CmdPanel.h"
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
    void _extractCmd();
    void _updatePressState();
    void _openDevice(std::string devicePath, bool blocking = false);
    void _clearBuffer();

    int _fd;
    LogitechJoystickMsg _msg;
    size_t _keyNum;
    std::vector<KeyPress> _keyStates;
    std::vector<KeyPress> _keyStatesPast;

};

#endif  // BIANLIB_LOGITECH_JOYSTICK_H