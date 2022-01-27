#ifndef BIANLIB_KEYACTION_H
#define BIANLIB_KEYACTION_H

#include <string>
#include <mathLib/mathTools.h>
#include <common/judgeEqual.h>

enum class KeyPress{
    RELEASE,
    PRESS,
    REPEAT
};

enum class ActionType{
    EMPTY,
    STATE,
    VALUE
};

struct KeyCmd{
    std::string c;
    KeyPress keyPress;
};

inline bool operator==(const KeyCmd &lhs, const KeyCmd &rhs){
    return strEqual(lhs.c, rhs.c) && lhs.keyPress == rhs.keyPress;
}

inline bool operator!=(const KeyCmd &lhs, const KeyCmd &rhs){
    return !(lhs == rhs);
}

class KeyAction{
public:
    KeyAction(ActionType type);
    virtual ~KeyAction(){};
    // virtual bool handleCmd(KeyCmd keyCmd, int &state, double &value) = 0;
    ActionType getType(){return _type;}
protected:
    ActionType _type;
};

class StateAction : public KeyAction{
public:
    StateAction(std::string c, int state, KeyPress press = KeyPress::PRESS);
    virtual ~StateAction(){};
    int getState(){return _state;};
    bool handleCmd(KeyCmd keyCmd, int &state);
protected:
    int _state;
    KeyCmd _keyCmdSet;
};

class EmptyAction : public StateAction{
public:
    EmptyAction(int state);
    ~EmptyAction(){};
    // bool handleCmd(KeyCmd keyCmd, int &state){};
private:

};

/* 
必须为长按, deltaValue为每秒改变量 
valueAction允许共用按键，例如空格停止
但是正反转、停止键不可重复
*/
class ValueAction : public KeyAction{
public:    
    ValueAction(std::string cUp, std::string cDown, double deltaValue, double initValue = 0.0);
    ValueAction(std::string cUp, std::string cDown, std::string cGoZero, double deltaValue, double initValue = 0.0);
    ValueAction(std::string cUp, std::string cDown, double deltaValue, double limit1, double limit2, double initValue = 0.0);
    ValueAction(std::string cUp, std::string cDown, std::string cGoZero, double deltaValue, double limit1, double limit2, double initValue = 0.0);

    ~ValueAction(){};
    bool handleCmd(KeyCmd keyCmd);
    void setDt(double dt);
    double getValue();
    double getDValue();
    double setValue(double value){_value = value;}
private:
    double _value;
    double _changeDirection;
    double _dV = 0.0;
    double _dt = 0.0;
    double _dVdt = 0.0;
    double _lim1, _lim2;
    bool _hasLim = false;
    bool _hasGoZero = false;

    KeyCmd _upCmdSet;
    KeyCmd _downCmdSet;
    KeyCmd _goZeroCmdSet;

};

/* 最初始的ValueAction，多档加减速 */
/* 可以考虑同一组按键，点按和长按意义不同 */
// class ShiftGearActon : ValueAction{

// }


#endif  // BIANLIB_KEYACTION_H