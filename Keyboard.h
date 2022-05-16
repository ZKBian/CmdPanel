/*
不区分大小写
*/

#ifndef BIANLIB_KEYBOARD_H
#define BIANLIB_KEYBOARD_H

// #include <linux/input.h>
#include <vector>
#include <string>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <sys/ioctl.h>
#include <mutex>
#include "multiThread/Loop.h"
#include "CmdPanel/include/CmdPanel.h"
#include "time/AbsoluteTimer.h"

// #define runTimeStep  0.002  // second, 500Hz
// #define readTimeStep 0.05   // second, this value cannot change

class Keyboard : public CmdPanel{
public:
    Keyboard(std::vector<KeyAction*> events, 
        EmptyAction emptyAction, size_t channelNum = 1, double dt = 0.002);
    ~Keyboard();
    std::string getString(std::string slogan);
private:
    void _read();
    void _pauseKey();
    void _startKey();
    void _extractCmd();

    fd_set _set;
    char _c = '\0';

    termios _oldSettings;
    termios _newSettings;
    timeval _tv;
};

#endif  // BIANLIB_KEYBOARD_H