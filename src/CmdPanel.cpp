#include "CmdPanel/CmdPanel.h"
#include "CmdPanel/include/KeyAction.h"

/***********************/
/*      KeyAction      */
/***********************/
KeyAction::KeyAction(ActionType type)
    : _type(type){}

StateAction::StateAction(std::string c, int state)
    :KeyAction(ActionType::STATE){
    _state = state;
    _keyCmdSet.c = c;
    _keyCmdSet.keyPress = KeyPress::PRESS;
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


ValueAction::ValueAction(std::string cUp, std::string cDown, double deltaValue, double initValue)
    : KeyAction(ActionType::VALUE), _dVdt(deltaValue), _value(initValue), 
      _hasLim(false), _hasGoZero(false){
    _upCmdSet.c = cUp;
    _upCmdSet.keyPress = KeyPress::REPEAT;
    _downCmdSet.c = cDown;
    _downCmdSet.keyPress = KeyPress::REPEAT;
}

ValueAction::ValueAction(std::string cUp, std::string cDown, double deltaValue, double limit1, 
    double limit2, double initValue)
    : ValueAction(cUp, cDown, deltaValue, initValue){
    _hasLim = true;
    _lim1 = limit1;
    _lim2 = limit2;
}

ValueAction::ValueAction(std::string cUp, std::string cDown, std::string cGoZero, double deltaValue, double initValue)
    : ValueAction(cUp, cDown, deltaValue, initValue){
    _hasGoZero = true;
    _goZeroCmdSet.c = cGoZero;
    _goZeroCmdSet.keyPress = KeyPress::PRESS;
}

ValueAction::ValueAction(std::string cUp, std::string cDown, std::string cGoZero, double deltaValue, 
    double limit1, double limit2, double initValue)
    : ValueAction(cUp, cDown, cGoZero, deltaValue, initValue){
    _hasLim = true;
    _lim1 = limit1;
    _lim2 = limit2;
}

void ValueAction::setDt(double dt){
    _dt = dt;
    _dV = _dVdt * _dt;
}

bool ValueAction::handleCmd(KeyCmd keyCmd){
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

double ValueAction::getValue(){
    if(_dV == 0.0){
        std::cout << "[ERROR] ValueAction::getValue, run setDt() before getValue()" << std::endl;
        exit(-1);
    }
// std::cout << "_changeDirection: " << _changeDirection << std::endl;
    _value += _changeDirection * _dV;

// std::cout << "_dV: " << _dV << std::endl
// << "_value: " << _value << std::endl;

    if(_hasLim){
        _value = saturation(_value, _lim1, _lim2);
    }
    return _value;
}

double ValueAction::getDValue(){
    if((_hasLim) && !inInterval(_value, _lim1, _lim2)){
        return 0.0;
    }

    return _changeDirection * _dVdt;
}

/***********************/
/*      CmdPanel       */
/***********************/

CmdPanel::CmdPanel(std::vector<KeyAction*> events, 
    EmptyAction emptyAction, double dt)
    : _emptyAction(emptyAction), _dt(dt){
    _state = _emptyAction.getState();
    _outputState = _state;

    _actionNum = events.size();
    for(int i(0); i<_actionNum; ++i){
        switch (events.at(i)->getType()){
        case ActionType::EMPTY:
            std::cout << "[ERROR] Keyboard::Keyboard, events cannot have EmptyAction" << std::endl;
            exit(-1);
            break;
        case ActionType::STATE:
            ++_stateNum;
            _stateEvents.push_back(*(StateAction*)events.at(i));
            break;
        case ActionType::VALUE:
            ++_valueNum;
            _valueEvents.push_back(*(ValueAction*)events.at(i));
            _valueEvents.at(_valueEvents.size()-1).setDt(dt);
            _values.push_back(_valueEvents.at(_valueEvents.size()-1).getValue());
            _dValues.push_back(_valueEvents.at(_valueEvents.size()-1).getDValue());
            break;
        default:
            break;
        }
    }
}

CmdPanel::~CmdPanel(){
    _running = false;
    delete _readThread;
    delete _runThread;
}

void CmdPanel::_start(){
    _readThread = new LoopFunc("CmdPanelRead", 0.0, boost::bind(&CmdPanel::_readStatic, this));
    _runThread = new LoopFunc("CmdPanelRun", _dt, boost::bind(&CmdPanel::_runStatic, this));

    _readThread->start();
    _runThread->start();
}

void CmdPanel::_run(){
    for(int i(0); i<_valueNum; ++i){
        _values.at(i) = _valueEvents.at(i).getValue();
        _dValues.at(i)= _valueEvents.at(i).getDValue();
    }
}

void CmdPanel::_updateState(){
    if((_outputState != _state) && _getState){
        _getState = false;
        _outputState = _state;
    }
}

void CmdPanel::_releaseKeyboard(){
    _keyCmd.c = "";
    _cPast = _keyCmd.c;
    
    _state = _emptyAction.getState();

    // valueAction需要停止命令
    for(int i(0); i<_valueNum; ++i){
        _valueEvents.at(i).handleCmd(_keyCmd);
    }
}

bool CmdPanel::_pressKeyboard(){
    if(strEqual(_keyCmd.c, _cPast)){
        _keyCmd.keyPress = KeyPress::REPEAT;
    }else{
        _keyCmd.keyPress = KeyPress::PRESS;
    }
    _cPast = _keyCmd.c;

    bool acted = false;
    // valueAction允许共用按键，例如空格停止
    for(int i(0); i<_valueNum; ++i){
        acted = acted || _valueEvents.at(i).handleCmd(_keyCmd);
    }

    for(int i(0); i<_stateNum; ++i){
        if(_stateEvents.at(i).handleCmd(_keyCmd, _state)){
            return true;
        }
    }

//     if(!acted){
// std::cout << "acted: " << (int)acted << std::endl;
//         _releaseKeyboard();
//     }

    return acted;
}


void* CmdPanel::_runStatic(void* obj){
    if(reinterpret_cast<CmdPanel*>(obj)->_running){
        reinterpret_cast<CmdPanel*>(obj)->_run();
    }
}

void* CmdPanel::_readStatic(void* obj){
    if(reinterpret_cast<CmdPanel*>(obj)->_running){
        reinterpret_cast<CmdPanel*>(obj)->_read();
    }
}

// 防止因为多线程，导致一个状态被错过
int CmdPanel::getState(){
    _getState = true;
    return _outputState;
}

std::vector<double> CmdPanel::getValues(){
    return _values;
}

std::vector<double> CmdPanel::getDValues(){
    return _dValues;
}

std::string getString(std::string slogan){
    std::cout << "Do not have function: getString()" << std::endl;
    return "NONE";
}
