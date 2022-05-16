#include <iostream>
#include <sstream>

// #include "common/executeCmd.h"
#include "CmdPanel/Keyboard.h"
#include "time/timeMarker.h"
#include "mathLib/mathTools.h"
#include "common/judgeEqual.h"

Keyboard::Keyboard(std::vector<KeyAction*> events, 
    EmptyAction emptyAction, size_t channelNum, double dt)
    : CmdPanel(events, emptyAction, channelNum, dt){

    tcgetattr( fileno( stdin ), &_oldSettings );
    _newSettings = _oldSettings;
    _oldSettings.c_lflag |= ( ICANON |  ECHO);
    _newSettings.c_lflag &= (~ICANON & ~ECHO);

    _startKey();
    _start();
}

Keyboard::~Keyboard(){
    _pauseKey();
}

void Keyboard::_extractCmd(){
    if(_c == '\033'){
        int m = read( fileno( stdin ), &_c, 1 );
        if(_c == '['){
            m = read( fileno( stdin ), &_c, 1 );
            switch (_c)
            {
            case 'A':
                _keyCmd.c = "up";
                break;
            case 'B':
                _keyCmd.c = "down";
                break;
            case 'C':
                _keyCmd.c = "right";
                break;
            case 'D':
                _keyCmd.c = "left";
                break;
            default:
                break;
            }
        }
    }
    else{
        _keyCmd.c = _c;
    }

    _pressKeyboard();
}

void Keyboard::_pauseKey(){
    tcsetattr( fileno( stdin ), TCSANOW, &_oldSettings );
    _running = false;
}

void Keyboard::_startKey(){
    tcsetattr( fileno( stdin ), TCSANOW, &_newSettings );
    _running = true;
}

void Keyboard::_read(){
    FD_ZERO(&_set);
    FD_SET( fileno( stdin ), &_set );

    _tv.tv_sec = 0;
    _tv.tv_usec = 40000;

    int res = 0;

    res = select( fileno( stdin )+1, &_set, NULL, NULL, &_tv);
    if(res > 0){
        int m = read( fileno( stdin ), &_c, 1 );
        _extractCmd();
    }else{
        _releaseKeyboard();
    }

    _updateState();
}

std::string Keyboard::getString(std::string slogan){
    _running = false;
    _pauseKey();

    std::string getString;
    std::cout << slogan << std::endl;
    std::getline(std::cin, getString);

    _startKey();
    _running = true;

    return getString;
}