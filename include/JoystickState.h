#ifndef BIANLIB_JOYSTICKSTATE_H
#define BIANLIB_JOYSTICKSTATE_H

#include "CmdPanel/include/KeyAction.h"

struct KeyState{
    KeyPress press;
    double time;  // ms
    // bool updated;
    std::string keyName;

    void init(std::string _keyName){
        keyName = _keyName;
        setZero();
    }

    void setZero(){
        press = KeyPress::RELEASE;
        time = 0;
        // updated = false;
    }
};

struct JoystickState
{
    const size_t A = 0;
    const size_t B = 1;
    const size_t X = 2;
    const size_t Y = 3;
    const size_t LB = 4;
    const size_t LT = 5;
    const size_t RB = 6;
    const size_t RT = 7;
    const size_t BACK = 8;
    const size_t START = 9;

    const size_t keyNum = 10;
    // size_t keyPressedNum = 0;
    std::vector<KeyState> keyStates = std::vector<KeyState>(10);

    double leftStickX;
    double leftStickY;
    double rightStickX;
    double rightStickY;
    double DpadX;
    double DpadY;

    JoystickState(){
        keyStates.at(A).init("A");
        keyStates.at(B).init("B");
        keyStates.at(X).init("X");
        keyStates.at(Y).init("Y");
        keyStates.at(LB).init("LB");
        keyStates.at(LT).init("LT");
        keyStates.at(RB).init("RB");
        keyStates.at(RT).init("RT");
        keyStates.at(BACK).init("BACK");
        keyStates.at(START).init("START");


        setZero();
    };

    ~JoystickState(){}

    void setZero(){
        for(int i(0); i<keyNum; i++){
            keyStates.at(i).setZero();
        }

        leftStickX = 0.0;
        leftStickY = 0.0;
        rightStickX = 0.0;
        rightStickY = 0.0;
        DpadX = 0.0;
        DpadY = 0.0;
    }

    bool otherKeyIs(size_t selfID, KeyPress goal){
        for(int i(0); i<keyNum; ++i){
            if((i != selfID) && (keyStates.at(i).press != goal)){
                return false;
            }
        }
        return true;
    }
};


#endif  // BIANLIB_JOYSTICKSTATE_H