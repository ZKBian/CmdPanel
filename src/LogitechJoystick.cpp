#include <fcntl.h>

#include "CmdPanel/LogitechJoystick.h"

LogitechJoystick::LogitechJoystick(std::string devicePath,
        std::vector<KeyAction*> events, 
        EmptyAction emptyAction, size_t channelNum, double dt)
        : CmdPanel(events, emptyAction, CmdPanelType::SWITCH_INPUT, channelNum, dt){
    _openDevice(devicePath);
    _init();
    _start();
}

LogitechJoystick::LogitechJoystick(std::vector<KeyAction*> events, 
        EmptyAction emptyAction, size_t channelNum, double dt)
        : CmdPanel(events, emptyAction, CmdPanelType::SWITCH_INPUT, channelNum, dt){
    _openDevice("/dev/input/js0");
    _init();
    _start();
}

LogitechJoystick::~LogitechJoystick(){
    close(_fd);
}

void LogitechJoystick::_openDevice(std::string devicePath, bool blocking){
    // Open the device using either blocking or non-blocking
    _fd = open(devicePath.c_str(), blocking ? O_RDONLY : O_RDONLY | O_NONBLOCK);
    if(_fd < 0){
        std::cout << "[WARNING] Cannot find joystick " << devicePath << std::endl;
    }else{
        std::cout << "[STATE] Found the joystick, fd = " << _fd << std::endl;
    }


}

void LogitechJoystick::_init(){
    _clearBuffer();

    _keyNum = 10;
    for(int i(0); i<_keyNum; ++i){
        _keyStates.push_back(KeyPress::RELEASE);
        _keyStatesPast.push_back(KeyPress::RELEASE);
    }
}

void LogitechJoystick::_clearBuffer(){
    while(true){
        _sample();
        if((_msg.isAxis()) && (_msg.number==5)){
            break;
        }
    }
}

bool LogitechJoystick::_sample(){
    int bytes = read(_fd, &_msg, sizeof(_msg)); 

    if (bytes == -1)
        return false;

    // NOTE if this condition is not met, we're probably out of sync and this
    // Joystick instance is likely unusable
// std::cout << "bytes: " << bytes << ", should be " << sizeof(_msg) << std::endl;
    return bytes == sizeof(_msg);
}

void LogitechJoystick::_read(){
    if(_sample()){
        _extractCmd();
        _pressKeyboard();
    }else{
        _releaseKeyboard();
    }

    // _updateState();
}

void LogitechJoystick::_extractCmd(){
    if(_msg.isButton()){
// std::cout << "butt number: " << (int)(_msg.number) << std::endl;
        switch ((int)_msg.number){
        case 0:
            _keyCmd.c = "X";
            break;
        case 1:
            _keyCmd.c = "A";
            break;
        case 2:
            _keyCmd.c = "B";
            break;
        case 3:
            _keyCmd.c = "Y";
            break;
        case 4:
            _keyCmd.c = "LB";
            break;            
        case 5:
            _keyCmd.c = "RB";
            break;
        case 6:
            _keyCmd.c = "LT";
            break;
        case 7:
            _keyCmd.c = "RT";
            break;
        case 8:
            _keyCmd.c = "BACK";
            break;
        case 9:
            _keyCmd.c = "START";
            break;
        default:
            break;
        }
        _updatePressState();
    }
    else if(_msg.isAxis()){
std::cout << "axis number: " << (int)(_msg.number) << std::endl;

    }

}

void LogitechJoystick::_updatePressState(){

}