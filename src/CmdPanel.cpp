#include "CmdPanel/include/CmdPanel.h"
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
    EmptyAction emptyAction, CmdPanelType type, size_t channelNum, double dt)
    : _emptyAction(emptyAction), _cmdPanelType(type), _channelNum(channelNum), _dt(dt){
    if(_channelNum < 1){
        std::cout << "[ERROR] CmdPanel::CmdPanel, _channelNum cannot smaller than 1" << std::endl;
        exit(-1);
    }

    _getState = std::vector<bool>(_channelNum, true);
    _outputState = std::vector<int>(_channelNum, _emptyAction.getState());
    _stateQueue.resize(_channelNum);

    _state = _emptyAction.getState();
    for(int i(0); i<_channelNum; ++i){
        _stateQueue.at(i).push_back(_state);
    }


// std::cout << "init _getState: " << _getState.size() << std::endl;
// std::cout << "init _outputState: " << _outputState.size() << std::endl;


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

    _keyCmd.c = "";
    _keyCmd.keyPress = KeyPress::RELEASE;
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

void CmdPanel::_updateStateValue(){
    // valueAction允许共用按键，例如空格停止
    for(int i(0); i<_valueNum; ++i){
        _valueEvents.at(i).handleCmd(_keyCmd);
    }

    _state = _emptyAction.getState();

    for(int i(0); i<_stateNum; ++i){
        if(_stateEvents.at(i).handleCmd(_keyCmd, _state)){
            break;
        }
    }

    for(int i(0); i<_channelNum; ++i){
        if((_stateQueue.at(i).size() == 0) ||
           (_stateQueue.at(i).back() != _state) ){
            _stateQueue.at(i).push_back(_state);
        }
    }
}

void CmdPanel::_releaseKeyboard(){
    if(_cmdPanelType == CmdPanelType::CONTINUE_INPUT){
        _keyCmd.c = "";
        _cPast = _keyCmd.c;
        _keyCmd.keyPress = KeyPress::RELEASE;
    }
    else if(_cmdPanelType == CmdPanelType::SWITCH_INPUT){
        if(_keyCmd.keyPress != KeyPress::RELEASE){
            _keyCmd.keyPress = KeyPress::REPEAT;
            _updateStateValue();
        }
    }

    _state = _emptyAction.getState();

    // valueAction需要停止命令
    for(int i(0); i<_valueNum; ++i){
        _valueEvents.at(i).handleCmd(_keyCmd);
    }
}

void CmdPanel::_pressKeyboard(){
    if(_cmdPanelType == CmdPanelType::CONTINUE_INPUT){
        if(strEqual(_keyCmd.c, _cPast)){
            _keyCmd.keyPress = KeyPress::REPEAT;
        }else{
            _keyCmd.keyPress = KeyPress::PRESS;
        }
        _cPast = _keyCmd.c;
    }
    else if(_cmdPanelType == CmdPanelType::SWITCH_INPUT){
        if(_keyCmd.keyPress != KeyPress::RELEASE){
            _keyCmd.keyPress = KeyPress::RELEASE;
        }else{
            _keyCmd.keyPress = KeyPress::PRESS;
        }
    }

    _updateStateValue();

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

// 防止因为多线程，导致一个状态被错过，在不同线程要用不同channelID
int CmdPanel::getState(size_t channelID){
// std::cout << "_getState size: " << _getState.size() << std::endl;
// std::cout << "_outputState size: " << _outputState.size() << std::endl;

    if(channelID > _channelNum-1){
        std::cout << "[ERROR] CmdPanel::getState(). The CmdPanel only has "
            << _channelNum << " channels, channelID cannot larger than "
            << _channelNum-1 << ", but it is setted to " << channelID << std::endl;
        exit(-1);
    }
    // _getState.at(channelID) = true;
    // int output = _outputState.at(channelID);
    // _outputState.at(channelID) = _emptyAction.getState();
    // return output;

    int output;
    if(_stateQueue.at(channelID).size() > 0){
        output = _stateQueue.at(channelID).at(0);
        _stateQueue.at(channelID).pop_front();
    }else{
        output = _emptyAction.getState();
    }
    return output;
}

std::vector<double> CmdPanel::getValues(){
    return _values;
}

std::vector<double> CmdPanel::getDValues(){
    return _dValues;
}

std::string CmdPanel::getString(std::string slogan){
    std::cout << "Do not have function: getString()" << std::endl;
    return "NONE";
}

void CmdPanel::setValue(std::vector<double> values){
    if(values.size() == _valueNum){
        for(int i(0); i<_valueNum; ++i){
            setValue(values.at(i), i);
        }
    }else{
        std::cout << "[ERROR] CmdPanel::setValue, the size of values is error" << std::endl;
    }
}

void CmdPanel::setValue(double value, size_t id){
    if(id >= _valueNum){
        std::cout << "[ERROR] CmdPanel::setValue, the id is " << id 
        << ", but the CmdPanel only has " << _valueNum << " values" << std::endl;
    }
    _values.at(id) = value;
    _dValues.at(id) = 0.0;
    _valueEvents.at(id).setValue(value);
}