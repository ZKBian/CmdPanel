#ifndef BIANLIB_UNITREEJOYSTICK_H
#define BIANLIB_UNITREEJOYSTICK_H

// currently used on B1 SDK
#include "CmdPanel/include/CmdPanel.h"
#include "UDPPort/UDPPort.h"
#include "CmdPanel/include/unitree_legged_sdk_B1/unitree_joystick.h"
#include "CmdPanel/include/unitree_legged_sdk_B1/comm.h"

using namespace UNITREE_LEGGED_SDK;

class UnitreeJoystick : public CmdPanel{
public:
    UnitreeJoystick(std::vector<KeyAction*> events, 
        EmptyAction emptyAction, size_t channelNum = 1,
        double dt = 0.002);
    ~UnitreeJoystick();
private:
    void _read();
    void _extractCmd();
    // void _pressKeyboard();

    xRockerBtnDataStruct _keyData;
    UDPPort *_udp;
    HighCmd _udpCmd;
    HighState _udpState;
};

#endif  // BIANLIB_UNITREEJOYSTICK_H