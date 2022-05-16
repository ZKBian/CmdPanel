#ifndef BIANLIB_LOGITECH_JOYSTICK_H
#define BIANLIB_LOGITECH_JOYSTICK_H

#include "CmdPanel/include/CmdPanel.h"

class LogitechJoystick : public CmdPanel{
public:
    LogitechJoystick(std::string devicePath, 
        std::vector<KeyAction*> events, 
        EmptyAction emptyAction, size_t channelNum = 1,
        double dt = 0.002);
    ~LogitechJoystick();
private:
    void _read()

};

#endif  // BIANLIB_LOGITECH_JOYSTICK_H