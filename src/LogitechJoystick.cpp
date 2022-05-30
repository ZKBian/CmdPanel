#include <fcntl.h>

#include "CmdPanel/LogitechJoystick.h"
#include "time/timeMarker.h"

LogitechJoystick::LogitechJoystick(std::string devicePath,
        std::vector<KeyAction*> events, 
        EmptyAction emptyAction, size_t channelNum, double dt)
        : CmdPanel(events, emptyAction, channelNum, dt){
    _openDevice(devicePath);
    _init();
    _start();
}

LogitechJoystick::LogitechJoystick(std::vector<KeyAction*> events, 
        EmptyAction emptyAction, size_t channelNum, double dt)
        : CmdPanel(events, emptyAction, channelNum, dt){
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

    _clickTime = 0.5;

    _combineKeyID.push_back(_keyStates.LB);
    _combineKeyID.push_back(_keyStates.LT);
    _combineKeyID.push_back(_keyStates.RB);
    _combineKeyID.push_back(_keyStates.RT);
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
    _updateJoystick();

// std::cout << "A: " << (int)_keyStates.keyStates.at(_keyStates.A).press << std::endl;
// std::cout << "left X: " << _keyStates.leftStickX << ", left Y: " << _keyStates.leftStickY << std::endl;
// std::cout << "right X: " << _keyStates.rightStickX << ", right Y: " << _keyStates.rightStickY << std::endl;
// std::cout << "Dpad X: " << _keyStates.DpadX << ", Dpad Y: " << _keyStates.DpadY << std::endl;


    if(_sample()){
        _extractMsg();
        // _pressKeyboard();
    }else{
        // _releaseKeyboard();
    }

    _checkCmdUpdated();
    _updateStateValue();
}

void LogitechJoystick::_extractMsg(){
    if(_msg.isButton()){
// std::cout << "butt number: " << (int)(_msg.number) << std::endl;
        switch ((int)_msg.number){
        case 0:
            _updatePressState(_keyStates.keyStates.at(_keyStates.X));
            break;
        case 1:
            _updatePressState(_keyStates.keyStates.at(_keyStates.A));
            break;
        case 2:
            _updatePressState(_keyStates.keyStates.at(_keyStates.B));
            break;
        case 3:
            _updatePressState(_keyStates.keyStates.at(_keyStates.Y));
            break;
        case 4:
            _updatePressState(_keyStates.keyStates.at(_keyStates.LB));
            break;            
        case 5:
            _updatePressState(_keyStates.keyStates.at(_keyStates.RB));
            break;
        case 6:
            _updatePressState(_keyStates.keyStates.at(_keyStates.LT));
            break;
        case 7:
            _updatePressState(_keyStates.keyStates.at(_keyStates.RT));
            break;
        case 8:
            _updatePressState(_keyStates.keyStates.at(_keyStates.BACK));
            break;
        case 9:
            _updatePressState(_keyStates.keyStates.at(_keyStates.START));
            break;
        default:
            break;
        }
    }
    else if(_msg.isAxis()){
        switch ((int)_msg.number){
        case 0:
            _keyStates.leftStickX = (_msg.value)/double(_msg.MAX_AXES_VALUE);
            break;
        case 1:
            _keyStates.leftStickY = -(_msg.value)/double(_msg.MAX_AXES_VALUE);
            break;
        case 2:
            _keyStates.rightStickX = (_msg.value)/double(_msg.MAX_AXES_VALUE);
            break;
        case 3:
            _keyStates.rightStickY = -(_msg.value)/double(_msg.MAX_AXES_VALUE);
            break;
        case 4:
            _keyStates.DpadX = (_msg.value)/double(_msg.MAX_AXES_VALUE);
            break;
        case 5:
            _keyStates.DpadY = -(_msg.value)/double(_msg.MAX_AXES_VALUE);
            break;
        default:
            break;
        }
        _updateStickAction();
    }

}

void LogitechJoystick::_updateJoystick(){
    for(int i(0); i<_keyStates.keyNum; ++i){
        _checkKeyRepeat(_keyStates.keyStates.at(i));
    }
}

void LogitechJoystick::_updatePressState(KeyState &currentState){
    currentState.time = getTimeSecond();
    switch (currentState.press){
    case KeyPress::RELEASE:
        currentState.press = KeyPress::PRESS;
        return;
    case KeyPress::PRESS:
    case KeyPress::REPEAT:
        currentState.press = KeyPress::RELEASE;
        _keyCmd.c = currentState.keyName;
        _keyCmd.keyPress = KeyPress::RELEASE;
        _updateStateValue();
        return;
    default:
        return;
    }
}

void LogitechJoystick::_checkKeyRepeat(KeyState &currentState){
    if(currentState.press == KeyPress::PRESS){
        if(getTimeSecond() - currentState.time > _clickTime){
            currentState.press = KeyPress::REPEAT;
        }
    }
}

void LogitechJoystick::_checkCmdUpdated(){
    /*single key*/
    for(int i(0); i<_keyStates.keyNum; ++i){
        if((_keyStates.keyStates.at(i).press != KeyPress::RELEASE)
            && _keyStates.otherKeyIs(i, KeyPress::RELEASE)){
            _keyCmd.c = _keyStates.keyStates.at(i).keyName;
            _keyCmd.keyPress = _keyStates.keyStates.at(i).press;
        }
    }
    _updateStateValue();

    /*two keys combined*/
    for(int j(0); j<_combineKeyID.size(); ++j){
        if((_keyStates.keyStates.at(_combineKeyID.at(j)).press != KeyPress::RELEASE)
            && !_keyStates.otherKeyIs(_combineKeyID.at(j), KeyPress::RELEASE)){
            for(int i(0); i<_keyStates.keyNum; ++i){
                if((_combineKeyID.at(j) != i) 
                    && (_keyStates.keyStates.at(i).press != KeyPress::RELEASE)){
                    _keyCmd.c = _keyStates.keyStates.at(_combineKeyID.at(j)).keyName 
                        + "_" + _keyStates.keyStates.at(i).keyName;
                    _keyCmd.keyPress = KeyPress::PRESS;
                }
            }
        }
    }
    _updateStateValue();

}

void LogitechJoystick::_updateStickAction(){
    for(int i(0); i<_joystickValueNum; ++i){
        if(_joystickValueEvents.at(i).getKeyName() == "LeftStickX"){
            _joystickValueEvents.at(i).updateStickValue(_keyStates.leftStickX);
// std::cout << "_keyStates.leftStickX: " << _keyStates.leftStickX << std::endl;
        }
        else if(_joystickValueEvents.at(i).getKeyName() == "LeftStickY"){
            _joystickValueEvents.at(i).updateStickValue(_keyStates.leftStickY);
        }
        else if(_joystickValueEvents.at(i).getKeyName() == "RightStickX"){
            _joystickValueEvents.at(i).updateStickValue(_keyStates.rightStickX);
        }
        else if(_joystickValueEvents.at(i).getKeyName() == "RightStickY"){
            _joystickValueEvents.at(i).updateStickValue(_keyStates.rightStickY);
        }
        else if(_joystickValueEvents.at(i).getKeyName() == "DpadX"){
            _joystickValueEvents.at(i).updateStickValue(_keyStates.DpadX);
        }
        else if(_joystickValueEvents.at(i).getKeyName() == "DpadY"){
            _joystickValueEvents.at(i).updateStickValue(_keyStates.DpadY);
        }
        else{
            std::cout << "[ERROR] LogitechJoystick::_updateStick, the stickName of JoystickValueAction can only be LeftStickX, LeftStickY, RightStickX, RightStickY, DpadX, DpadY" << std::endl;
        }
    }
}