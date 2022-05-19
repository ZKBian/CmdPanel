#include "CmdPanel/include/KeyAction.h"

/***********************/
/*      KeyAction      */
/***********************/
KeyAction::KeyAction(ActionType type)
    : _type(type){}

StateAction::StateAction(std::string c, int state, KeyPress press)
    :KeyAction(ActionType::STATE){
    _state = state;
    _keyCmdSet.c = c;
    _keyCmdSet.keyPress = press;
}

bool StateAction::handleCmd(KeyCmd keyCmd, int &state){
    if(keyCmd == _keyCmdSet){
        state = _state;
        return true;
    }else{
        return false;
    }
}

EmptyAction::EmptyAction(int state)
    : StateAction("\0", state){
    _type = ActionType::EMPTY;
}


KeyValueAction::KeyValueAction(std::string cUp, std::string cDown, double deltaValue, double initValue)
    : ValueAction(ActionType::KEYVALUE), _dVdt(deltaValue), _hasLim(false), _hasGoZero(false){
    _upCmdSet.c = cUp;
    _upCmdSet.keyPress = KeyPress::REPEAT;
    _downCmdSet.c = cDown;
    _downCmdSet.keyPress = KeyPress::REPEAT;

    _value = initValue;
}

KeyValueAction::KeyValueAction(std::string cUp, std::string cDown, double deltaValue, double limit1, 
    double limit2, double initValue)
    : KeyValueAction(cUp, cDown, deltaValue, initValue){
    _hasLim = true;
    _lim1 = limit1;
    _lim2 = limit2;
}

KeyValueAction::KeyValueAction(std::string cUp, std::string cDown, std::string cGoZero, double deltaValue, double initValue)
    : KeyValueAction(cUp, cDown, deltaValue, initValue){
    _hasGoZero = true;
    _goZeroCmdSet.c = cGoZero;
    _goZeroCmdSet.keyPress = KeyPress::PRESS;
}

KeyValueAction::KeyValueAction(std::string cUp, std::string cDown, std::string cGoZero, double deltaValue, 
    double limit1, double limit2, double initValue)
    : KeyValueAction(cUp, cDown, cGoZero, deltaValue, initValue){
    _hasLim = true;
    _lim1 = limit1;
    _lim2 = limit2;
}

void KeyValueAction::setDt(double dt){
    _dt = dt;
    _dV = _dVdt * _dt;
}

bool KeyValueAction::handleCmd(KeyCmd keyCmd){
    if(keyCmd == _upCmdSet){
// std::cout << "up" << std::endl;
        _changeDirection = 1.0;
        return true;
    }
    else if(keyCmd == _downCmdSet){
// std::cout << "down" << std::endl;
        _changeDirection = -1.0;
        return true;
    }
    else if(keyCmd == _goZeroCmdSet){
        if(_hasGoZero){
            _value = 0.0;
            _changeDirection = 0.0;
            return true;
        }
    }else{
// std::cout << "stop" << std::endl;
        _changeDirection = 0.0;
        return false;
    }
}

double KeyValueAction::getValue(){
    if(_dV == 0.0){
        std::cout << "[ERROR] KeyValueAction::getValue, run setDt() before getValue()" << std::endl;
        exit(-1);
    }
    _value += _changeDirection * _dV;

    if(_hasLim){
        _value = saturation(_value, _lim1, _lim2);
    }
    return _value;
}

double KeyValueAction::getDValue(){
    if((_hasLim) && !inInterval(_value, _lim1, _lim2)){
        return 0.0;
    }

    return _changeDirection * _dVdt;
}


JoystickValueAction::JoystickValueAction(std::string stickName, double limit1, double limit2)
    : ValueAction(ActionType::JOYSTICKVALUE), _stickName(stickName){
    if(limit1*limit2 > 0){
        std::cout << "[ERROR] JoystickValueAction::JoystickValueAction, the limits should have one positive value and one negative" << std::endl;
        exit(-1);
    }

    if(limit2 > limit1){
        _limNegative = limit1;
        _limPositive = limit2;
    }else{
        _limNegative = limit2;
        _limPositive = limit1;
    }
}

double JoystickValueAction::getValue(){
    if(_stickValue >= 0){
        _value = _stickValue * _limPositive;
    }else{
        _value = _stickValue * _limNegative;
    }

    return _value;
}