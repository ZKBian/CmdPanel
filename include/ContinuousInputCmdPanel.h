#ifndef BIANLIB_CONTINUOUS_INPUT_CMDPANEL_H
#define BIANLIB_CONTINUOUS_INPUT_CMDPANEL_H

#include "CmdPanel/include/CmdPanel.h"

/*
Like keyboard, unitree joystick,
the commands come continuously.
As for Logitech joystick, 
you can only receive commands 
when pressing or releasing a key.
*/
class ContinuousInputCmdPanel : public CmdPanel{
public:
    ContinuousInputCmdPanel(std::vector<KeyAction*> events, 
        EmptyAction emptyAction, 
        size_t channelNum = 1, double dt = 0.002);
    virtual ~ContinuousInputCmdPanel();
protected:
    void _pressKeyboard();
    void _releaseKeyboard();


};

#endif  // BIANLIB_CONTINUOUS_INPUT_CMDPANEL_H