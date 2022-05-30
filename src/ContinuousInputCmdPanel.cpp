#include "CmdPanel/include/ContinuousInputCmdPanel.h"

ContinuousInputCmdPanel::ContinuousInputCmdPanel(
    std::vector<KeyAction*> events, 
    EmptyAction emptyAction, 
    size_t channelNum, double dt)
    :CmdPanel(events, emptyAction, channelNum, dt){

}

ContinuousInputCmdPanel::~ContinuousInputCmdPanel(){
}

void ContinuousInputCmdPanel::_releaseKeyboard(){
    _keyCmd.c = "";
    _cPast = _keyCmd.c;
    _keyCmd.keyPress = KeyPress::RELEASE;

    _state = _emptyAction.getState();

    // valueAction需要停止命令
    for(int i(0); i<_keyValueNum; ++i){
        _keyValueEvents.at(i).handleCmd(_keyCmd);
    }

    _updateStateValue();
}

void ContinuousInputCmdPanel::_pressKeyboard(){
    if(strEqual(_keyCmd.c, _cPast)){
        _keyCmd.keyPress = KeyPress::REPEAT;
    }else{
        _keyCmd.keyPress = KeyPress::PRESS;
    }
    _cPast = _keyCmd.c;

    _updateStateValue();
}