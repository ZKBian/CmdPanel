#include "CmdPanel/LogitechJoystick.h"
#include <iostream>

enum class TestEnum{
    enum1,
    enum2,
    enum3,
    none
};

void func0(LogitechJoystick* key){
    // key->getState(0);
    std::cout << "func0 state: " << key->getState(0) << std::endl;
    std::cout << "value0: " << key->getValues().at(0)
        << ", value1: " << key->getValues().at(1) << std::endl;
}

void func1(LogitechJoystick* key){
    std::cout << "func1 state: " << key->getState(1) << std::endl;
}

int main(){
    std::vector<KeyAction*> action;
    action.push_back(new StateAction("A", (int)TestEnum::enum1));
    action.push_back(new StateAction("RT_B", (int)TestEnum::enum2));
    action.push_back(new StateAction("LB", (int)TestEnum::enum3));
    action.push_back(new JoystickValueAction("LeftStickX"));
    action.push_back(new KeyValueAction("A", "B", 1.0));

    EmptyAction emptyAction((int)TestEnum::none);

    LogitechJoystick joystick(action, emptyAction, 2);
    std::vector<double> values;
    std::vector<double> dValues;

    /*
        multi-thread
    */
    LoopFunc loop0("loop0", 0.5, boost::bind(&func0, &joystick));
    LoopFunc loop1("loop1", 0.5, boost::bind(&func1, &joystick));

    loop0.start();
    loop1.start();

    while(true){}

    return 0;
}