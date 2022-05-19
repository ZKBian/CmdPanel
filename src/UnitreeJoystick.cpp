#include "CmdPanel/UnitreeJoystick.h"

UnitreeJoystick::UnitreeJoystick(std::vector<KeyAction*> events, 
    EmptyAction emptyAction, size_t channelNum, double dt)
    : ContinuousInputCmdPanel(events, emptyAction, channelNum, dt){
    _udp = new UDPPort("192.168.123.220", 8082, 8081, sizeof(HighState), BlockYN::NO, 500000);
    // _udp = new UDPPort("192.168.123.220", 8082, 8081, 143, BlockYN::NO, 500000);

    _udpCmd = {0};
    _udpState = {0};

    // _udp->InitCmdData(_udpCmd);
    _start();
}

UnitreeJoystick::~UnitreeJoystick(){
    delete _udp;
}

void UnitreeJoystick::_read(){
// std::cout << "start read" << std::endl;
    _udp->send((uint8_t*)&_udpCmd, sizeof(HighCmd));
    _udp->recv((uint8_t*)&_udpState);

    memcpy(&_keyData, _udpState.wirelessRemote, 40);

// std::cout << "left: " << (int)_keyData.btn.components.left << std::endl;

    _extractCmd();
    // _updateState();

    // usleep(2000);
}

void UnitreeJoystick::_extractCmd(){
    if(((int)_keyData.btn.components.left  == 1)
        && ((int)_keyData.btn.components.L1 == 0)
        && ((int)_keyData.btn.components.L2 == 0)
        && ((int)_keyData.btn.components.R1 == 0)
        && ((int)_keyData.btn.components.R2 == 0)){
        _keyCmd.c = "left";
    }
    else if(((int)_keyData.btn.components.right  == 1)
        && ((int)_keyData.btn.components.L1 == 0)
        && ((int)_keyData.btn.components.L2 == 0)
        && ((int)_keyData.btn.components.R1 == 0)
        && ((int)_keyData.btn.components.R2 == 0)){
        _keyCmd.c = "right";
    }
    else if(((int)_keyData.btn.components.up  == 1)
        && ((int)_keyData.btn.components.L1 == 0)
        && ((int)_keyData.btn.components.L2 == 0)
        && ((int)_keyData.btn.components.R1 == 0)
        && ((int)_keyData.btn.components.R2 == 0)){
        _keyCmd.c = "up";
    }
    else if(((int)_keyData.btn.components.down  == 1)
        && ((int)_keyData.btn.components.L1 == 0)
        && ((int)_keyData.btn.components.L2 == 0)
        && ((int)_keyData.btn.components.R1 == 0)
        && ((int)_keyData.btn.components.R2 == 0)){
        _keyCmd.c = "down";
    }
    else if(((int)_keyData.btn.components.select  == 1)
        && ((int)_keyData.btn.components.L1 == 0)
        && ((int)_keyData.btn.components.L2 == 0)
        && ((int)_keyData.btn.components.R1 == 0)
        && ((int)_keyData.btn.components.R2 == 0)){
        _keyCmd.c = "select";
    }
    else if(((int)_keyData.btn.components.L1 == 0)
        && ((int)_keyData.btn.components.L2 == 0)
        && ((int)_keyData.btn.components.R1 == 1)
        && ((int)_keyData.btn.components.R2 == 0)){
        _keyCmd.c = "r1";
    }
    else if(((int)_keyData.btn.components.L1 == 0)
        && ((int)_keyData.btn.components.L2 == 0)
        && ((int)_keyData.btn.components.R1 == 0)
        && ((int)_keyData.btn.components.R2 == 1)){
        _keyCmd.c = "r2";
    }
    else if(((int)_keyData.btn.components.X == 1)
        && ((int)_keyData.btn.components.L1 == 0)
        && ((int)_keyData.btn.components.L2 == 1)
        && ((int)_keyData.btn.components.R1 == 0)
        && ((int)_keyData.btn.components.R2 == 0)){
        _keyCmd.c = "l2_x";
    }
    else{
        _releaseKeyboard();
        return;
    }

    _pressKeyboard();
}
