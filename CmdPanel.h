#ifndef BIANLIB_CMDPANEL_H
#define BIANLIB_CMDPANEL_H

#include <vector>
#include "CmdPanel/include/KeyAction.h"
#include "multiThread/Loop.h"

class CmdPanel{
public:
    CmdPanel(std::vector<KeyAction*> events, 
        EmptyAction emptyAction, size_t channelNum = 1, double dt = 0.002);
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
    void _updateState();
    bool _pressKeyboard();
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
    std::vector<int> _outputState;
    std::vector<bool> _getState;
    double _dt;
    KeyCmd _keyCmd;
    std::string _cPast = "";

    bool _running = true;
};

#endif  // BIANLIB_CMDPANEL_H