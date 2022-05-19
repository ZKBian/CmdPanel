#ifndef BIANLIB_CMDPANEL_H
#define BIANLIB_CMDPANEL_H

#include <vector>
#include <deque>
#include "CmdPanel/include/KeyAction.h"
#include "multiThread/Loop.h"

// enum class CmdPanelType{
//     CONTINUE_INPUT,         // Once press a key, the command will be received continuously
//     SWITCH_INPUT            // One press has two commands, one for press, one for release
// };

class CmdPanel{
public:
    CmdPanel(std::vector<KeyAction*> events, 
        EmptyAction emptyAction, 
        size_t channelNum = 1, double dt = 0.002);
    virtual ~CmdPanel();
    int getState(size_t channelID = 0);
    std::vector<double> getValues();
    std::vector<double> getDValues();
    void setValue(std::vector<double> values);
    void setValue(double value, size_t id);
    virtual std::string getString(std::string slogan);
protected:
    void _start();
    void _updateStateValue();

    virtual void _read() = 0;
    static void* _readStatic(void* obj);
    LoopFunc *_readThread;

    void _run();
    static void* _runStatic(void* obj);
    LoopFunc *_runThread;

    /*为了防止Action被销毁，这里没有用指针，因此不可以使用多态*/
    std::vector<StateAction> _stateEvents;
    std::vector<KeyValueAction> _keyValueEvents;
    std::vector<JoystickValueAction> _joystickValueEvents;

    EmptyAction _emptyAction;
    size_t _actionNum = 0;
    size_t _stateNum = 0;
    size_t _valueTotalNum = 0;
    size_t _keyValueNum = 0;
    size_t _joystickValueNum = 0;
    size_t _channelNum;
    std::vector<double> _values;
    std::vector<double> _dValues;
    int _state;
    std::vector<std::deque<int>> _stateQueue;
    KeyCmd _keyCmd;
    std::string _cPast = "";

    bool _running = true;
    double _dt;
};

#endif  // BIANLIB_CMDPANEL_H