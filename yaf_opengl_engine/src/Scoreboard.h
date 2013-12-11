#ifndef SCOREBOARD_H_
#define SCOREBOARD_H_

#include <string>
#include <windows.h>
#include <gl/glew.h>
#include <gl/freeglut.h>
#include <gl/glui.h>

#include "YafMisc.h"

class Scoreboard
{
public:
    Scoreboard();
    void Update(uint millis);
    void IncHome();
    void IncAway();
    void Reset();
    void Draw();
private:
    uint _home;
    uint _away;
    uint _firstMillis;
    std::string _currentTime;
};

#endif