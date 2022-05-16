#include <iostream>
#include <vector>
#include <string>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <sys/ioctl.h>
#include <mutex>
#include <string.h>
#include <string>

/*
    final test
*/
#include "CmdPanel/Keyboard.h"
enum class TestEnum{
    enum1,
    enum2,
    enum3,
    none
};

void func0(Keyboard* key){
    // key->getState(0);
    std::cout << "func0 state: " << key->getState(0) << std::endl;
}

void func1(Keyboard* key){
    std::cout << "func1 state: " << key->getState(1) << std::endl;
}

int main(){
    std::vector<KeyAction*> action;
    action.push_back(new StateAction("r", (int)TestEnum::enum1));
    action.push_back(new StateAction("f", (int)TestEnum::enum2));
    action.push_back(new StateAction("v", (int)TestEnum::enum3));
    action.push_back(new ValueAction("q", "a", "z", 1.0, -3.0, 2.0));
    action.push_back(new ValueAction("up", "down", 1.0));

    EmptyAction emptyAction((int)TestEnum::none);

    Keyboard keyboard(action, emptyAction, 2);
    std::vector<double> values;
    std::vector<double> dValues;

    /*
        multi-thread
    */
    LoopFunc loop0("loop0", 0.5, boost::bind(&func0, &keyboard));
    LoopFunc loop1("loop1", 0.5, boost::bind(&func1, &keyboard));

    loop0.start();
    loop1.start();

    while(true){}


    /*
        normal test
    */
    // TestEnum stateName = TestEnum::none;
    // while(stateName != TestEnum::enum3){
    //     stateName = (TestEnum)keyboard.getState();
    //     std::cout << "state: " << (int)stateName << std::endl;

    //     values = keyboard.getValues();
    //     dValues= keyboard.getDValues();

    //     std::cout << "q : " << values.at(0) << ", " << values.at(1) << std::endl;
    //     std::cout << "qd: " << dValues.at(0) << ", " << dValues.at(1) << std::endl;

    //     usleep(500000);
    // }

    // std::string getS = keyboard.getString("input: ");
    // std::cout << "get: " << getS << std::endl;

    return 0;
}


/*
    Test std::string and char
*/
// int main(){
//     std::string str;
//     char c = 'a';

//     str = c;

//     std::cout << str << std::endl;
// }

/*
    Test class arguments
*/
// class father{
// public:
//     int getValue(){return value;}
//     int value;
// };

// class son1 : public father{
// public:
//     son1(){value = 1;}
// };

// class son2 : public father{
// public:
//     son2(){value = 2;}
// };

// class group{
// public:
//     group(std::vector<father*> input){
//         s1.push_back(*(son1*)input.at(0));
//         s2.push_back(*(son2*)input.at(1));
//         f.push_back(*input.at(0));
//         f.push_back(*input.at(1));
//     }
//     void test(){
//         std::cout << groupValue << std::endl;
//         std::cout << s1.at(0).getValue() << std::endl;
//         std::cout << s2.at(0).getValue() << std::endl;

//         std::cout << f.at(0).getValue() << std::endl;
//         std::cout << f.at(1).getValue() << std::endl;
//     }
// private:
//     std::vector<son1> s1;
//     std::vector<son2> s2;
//     std::vector<father> f;

//     int groupValue = 4;
// };

// group *_group;

// void inputArgu(){
//     std::vector<father*> vec;
//     vec.push_back(new son1());
//     vec.push_back(new son2());
//     _group = new group(vec);
// }

// int main(){
//     inputArgu();
//     _group->test();
// }



/*
    Test char equal compare
*/
// bool charEqual(char a, char b, bool caseMatter = false){
//     if(caseMatter){
//         return a == b;
//     }else{
//         char aStr[2] = {a, '\0'};
//         char bStr[2] = {b, '\0'};
//         return strcasecmp(aStr, bStr) == 0;
//     }
// }
// int main(){
//     char a = '\0';
//     char b = 'c';

//     if(charEqual(a, b)){
//         std::cout << "equal" << std::endl;
//     }else{
//         std::cout << "not equal" << std::endl;
//     }
// }


// inline bool cStrEqual(const char *a, const char *b, bool caseMatter = false){
//     if(caseMatter){
//         return strcmp(a, b) == 0;
//     }else{
//         return strcasecmp(a, b) == 0;
//     }
// }
// #include <string>
// int main(){
//     std::string a = " ";
//     std::string b = " ";

//     if(cStrEqual(a.c_str(), b.c_str())){
//         std::cout << "equal" << std::endl;
//     }else{
//         std::cout << "not equal" << std::endl;
//     }
// }



/*
    Test stdin
*/
// int main(){
//     termios _oldSettings;
//     termios _newSettings;

//     tcgetattr( fileno( stdin ), &_oldSettings );
//     _newSettings = _oldSettings;
//     _oldSettings.c_lflag |= ( ICANON |  ECHO);
//     _newSettings.c_lflag &= (~ICANON & ~ECHO);

//     tcsetattr( fileno( stdin ), TCSANOW, &_newSettings );

//     fd_set _set;
//     timeval _tv;

//     while(true){
//         FD_ZERO(&_set);
//         FD_SET( fileno( stdin ), &_set );

//         _tv.tv_sec = 0;
//         _tv.tv_usec = 50000;

//         int res = 0;
//         char c = '\0';

//         res = select( fileno( stdin )+1, &_set, NULL, NULL, &_tv);
//         if(res > 0){
//             int m = read( fileno( stdin ), &c, 1 );
//         }
//         std::cout << "res: " << res << ", c: " << c << std::endl;

//     }

//     tcsetattr( fileno( stdin ), TCSANOW, &_oldSettings );

//     return 0;
// }


/*
    Test c string
*/
// int main(){
//     // const char* cmd = "some " "thing";
//     // std::string cmdStr = cmd;
//     // std::cout << cmdStr << std::endl;
//     Keyboard key;

//     while(true){usleep(10000);}
//     return 0;
// }