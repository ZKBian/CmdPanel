#include "CmdPanel/include/CmdPanel.h"
#include "CmdPanel/include/KeyAction.h"

/***********************/
/*      CmdPanel       */
/***********************/

CmdPanel::CmdPanel(std::vector<KeyAction*> events, 
    EmptyAction emptyAction, size_t channelNum, double dt)
    : _emptyAction(emptyAction), _channelNum(channelNum), _dt(dt){
    if(_channelNum < 1){
        std::cout << "[ERROR] CmdPanel::CmdPanel, _channelNum cannot smaller than 1" << std::endl;
        exit(-1);
    }

    _stateQueue.resize(_channelNum);

    _state = _emptyAction.getState();
    for(int i(0); i<_channelNum; ++i){
        _stateQueue.at(i).push_back(_state);
    }

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
        case ActionType::KEYVALUE:
            _keyValueEvents.push_back(*(KeyValueAction*)events.at(i));
            _keyValueEvents.at(_keyValueEvents.size()-1).setDt(dt);
            _keyValueEvents.at(_keyValueEvents.size()-1).setID(_valueTotalNum);
            _values.push_back(_keyValueEvents.at(_keyValueEvents.size()-1).getValue());
            _dValues.push_back(_keyValueEvents.at(_keyValueEvents.size()-1).getDValue());
            ++_keyValueNum;
            ++_valueTotalNum;
            break;
        case ActionType::JOYSTICKVALUE:
            _joystickValueEvents.push_back(*(JoystickValueAction*)events.at(i));
            _joystickValueEvents.at(_joystickValueEvents.size()-1).setID(_valueTotalNum);
            _values.push_back(_joystickValueEvents.at(_joystickValueEvents.size()-1).getValue());
            _dValues.push_back(0.0);
            ++_joystickValueNum;
            ++_valueTotalNum;
        default:
            break;
        }
    }

    // _valueTotalNum = _keyValueNum + _joystickValueNum;

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

void CmdPanel::_updateStateValue(){
    // valueAction允许共用按键，例如空格停止
    for(int i(0); i<_keyValueNum; ++i){
        _keyValueEvents.at(i).handleCmd(_keyCmd);
    }

    // state
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

void* CmdPanel::_readStatic(void* obj){
    if(reinterpret_cast<CmdPanel*>(obj)->_running){
        reinterpret_cast<CmdPanel*>(obj)->_read();
    }
}

// 防止因为多线程，导致一个状态被错过，在不同线程要用不同channelID
int CmdPanel::getState(size_t channelID){

    if(channelID > _channelNum-1){
        std::cout << "[ERROR] CmdPanel::getState(). The CmdPanel only has "
            << _channelNum << " channels, channelID cannot larger than "
            << _channelNum-1 << ", but it is setted to " << channelID << std::endl;
        exit(-1);
    }

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
    if(values.size() == _valueTotalNum){
        for(int i(0); i<_valueTotalNum; ++i){
            setValue(values.at(i), i);
        }
    }else{
        std::cout << "[ERROR] CmdPanel::setValue, the size of values is error" << std::endl;
    }
}

void CmdPanel::setValue(double value, size_t id){
    if(id >= _valueTotalNum){
        std::cout << "[ERROR] CmdPanel::setValue, the id is " << id 
        << ", but the CmdPanel only has " << _valueTotalNum << " values" << std::endl;
    }
    _values.at(id) = value;
    _dValues.at(id) = 0.0;

    for(int i(0); i<_keyValueNum; ++i){
        if(_keyValueEvents.at(id).getID() == id){
            _keyValueEvents.at(id).setValue(value);
            break;
        }
    }
}

void CmdPanel::_run(){
    for(int i(0); i<_keyValueNum; ++i){
        _values.at(_keyValueEvents.at(i).getID()) = _keyValueEvents.at(i).getValue();
        _dValues.at(_keyValueEvents.at(i).getID())= _keyValueEvents.at(i).getDValue();
    }
    for(int i(0); i<_joystickValueNum; ++i){
        _values.at(_joystickValueEvents.at(i).getID()) = _joystickValueEvents.at(i).getValue();
        _dValues.at(_joystickValueEvents.at(i).getID()) = 0.0;
    }
}

void* CmdPanel::_runStatic(void* obj){
    if(reinterpret_cast<CmdPanel*>(obj)->_running){
        reinterpret_cast<CmdPanel*>(obj)->_run();
    }
}