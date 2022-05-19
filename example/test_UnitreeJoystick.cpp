#include "CmdPanel/UnitreeJoystick.h"

enum class TestEnum{
    enum1,
    enum2,
    enum3,
    enum4,
    enum5,
    none
};

int main(){
    std::vector<KeyAction*> action;
    action.push_back(new StateAction("select", (int)TestEnum::enum1));
    action.push_back(new StateAction("left", (int)TestEnum::enum2));
    action.push_back(new StateAction("right", (int)TestEnum::enum3));
    action.push_back(new StateAction("r1", (int)TestEnum::enum4));
    action.push_back(new StateAction("r2", (int)TestEnum::enum5));

    action.push_back(new KeyValueAction("q", "a", "z", 1.0, -3.0, 2.0));
    action.push_back(new KeyValueAction("up", "down", 1.0));

    EmptyAction emptyAction((int)TestEnum::none);

    UnitreeJoystick joystick(action, emptyAction);

    std::vector<double> values;
    std::vector<double> dValues;

    while(true){
        std::cout << "state: " << joystick.getState() << std::endl;

        values = joystick.getValues();
        dValues= joystick.getDValues();

        // std::cout << "q : " << values.at(0) << ", " << values.at(1) << std::endl;
        // std::cout << "qd: " << dValues.at(0) << ", " << dValues.at(1) << std::endl;

        usleep(500000);
    }

    return 0;
}