#ifndef BIANLIB_KEYACTION_H
#define BIANLIB_KEYACTION_H

#include <string>
// #include <mathLib/mathTools.h>
#include <common/judgeEqual.h>

enum class KeyPress{
    RELEASE,
    PRESS,
    REPEAT
};

enum class ActionType{
    EMPTY,
    STATE,
    KEYVALUE,
    JOYSTICKVALUE
};

enum class JoystickKey{
    LEFT_STICK,
    RIGHT_STICK,
    D_PAD,

    // LB,
    // RB,
    // LT,
    // RT,
    // BACK,
    // START,
    // A,
    // B,
    // X,
    // Y
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
抽象类，包含按键改变数值与摇杆改变数值
*/
class ValueAction : public KeyAction{
public:
    ValueAction(ActionType type):KeyAction(type){};
    virtual ~ValueAction(){}

    virtual bool handleCmd(KeyCmd keyCmd){return false;};
    virtual void setDt(double dt){};
    virtual double getValue(){return 0.0;};
    virtual double getDValue(){return 0.0;};
    virtual void setValue(double value){};
    virtual std::string getKeyName(){return "NONE";};
    virtual void updateStickValue(double stickValue){};

    void setID(size_t id){_valueID = id;}
    size_t getID(){return _valueID;}
protected:
    double _value;
    size_t _valueID;
};

/* 
必须为长按, deltaValue为每秒改变量 
valueAction允许共用按键，例如空格停止
但是正反转、停止键不可重复
*/
class KeyValueAction : public ValueAction{
public:    
    KeyValueAction(std::string cUp, std::string cDown, double deltaValue, double initValue = 0.0);
    KeyValueAction(std::string cUp, std::string cDown, std::string cGoZero, double deltaValue, double initValue = 0.0);
    KeyValueAction(std::string cUp, std::string cDown, double deltaValue, double limit1, double limit2, double initValue = 0.0);
    KeyValueAction(std::string cUp, std::string cDown, std::string cGoZero, double deltaValue, double limit1, double limit2, double initValue = 0.0);

    ~KeyValueAction(){};

    bool handleCmd(KeyCmd keyCmd);
    void setDt(double dt);
    double getValue();
    double getDValue();
    void setValue(double value){_value = value;}
    std::string getKeyName(){return _upCmdSet.c + "_" + _downCmdSet.c + "_" + _goZeroCmdSet.c;}
private:
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

/*
适用于手柄上的摇杆
*/
class JoystickValueAction : public ValueAction{
public:
    JoystickValueAction(std::string stickName, double limit1 = -1.0, double limit2 = 1.0);
    ~JoystickValueAction(){};

    double getValue();
    std::string getKeyName(){return _stickName;}
    void updateStickValue(double stickValue){_stickValue = stickValue;};

private:
    double _limNegative;
    double _limPositive;

    std::string _stickName;
    double _stickValue;     // [-1, 1]
};


/* 最初始的ValueAction，多档加减速 */
/* 可以考虑同一组按键，点按和长按意义不同 */
// class ShiftGearActon : ValueAction{

// }


#endif  // BIANLIB_KEYACTION_H