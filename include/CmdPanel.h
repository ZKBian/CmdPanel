#ifndef BIANLIB_CMDPANEL_H
#define BIANLIB_CMDPANEL_H

#include <vector>
#include <deque>
#include "CmdPanel/include/KeyAction.h"
#include "multiThread/Loop.h"

enum class CmdPanelType{
    CONTINUE_INPUT,         // Once press a key, the command will be received continuously
    SWITCH_INPUT            // One press has two commands, one for press, one for release
};

class CmdPanel{
public:
    CmdPanel(std::vector<KeyAction*> events, 
        EmptyAction emptyAction, CmdPanelType type, 
        size_t channelNum = 1, double dt = 0.002);
    virtual ~CmdPanel();
    int getState(size_t channelID = 0);
    std::vector<double> getValues();
    std::vector<double> getDValues();
    void setValue(std::vector<double> values);
    void setValue(double value, size_t id);
    virtual std::string getString(std::string slogan);
protected:
    virtual void _read() = 0;
    // virtual void _pressKeyboard() = 0;
    // void _releaseKeyboard();

    void _start();
    void _run();
    void _updateStateValue();
    void _pressKeyboard();
    void _releaseKeyboard();

    static void* _runStatic(void* obj);
    static void* _readStatic(void* obj);

    LoopFunc *_runThread;
    LoopFunc *_readThread;

    std::vector<StateAction> _stateEvents;
    std::vector<ValueAction> _valueEvents;

    EmptyAction _emptyAction;
    size_t _actionNum = 0;
    size_t _stateNum = 0;
    size_t _valueNum = 0;
    size_t _channelNum;
    std::vector<double> _values;
    std::vector<double> _dValues;
    int _state;
    std::vector<std::deque<int>> _stateQueue;
    std::vector<int> _outputState;
    std::vector<bool> _getState;
    double _dt;
    KeyCmd _keyCmd;
    std::string _cPast = "";
    // KeyPress _pressPast = KeyPress::RELEASE;

    bool _running = true;
    CmdPanelType _cmdPanelType;
};

#endif  // BIANLIB_CMDPANEL_H