#ifndef SCOREBOARD_H_
#define SCOREBOARD_H_

#include <string>
#include "YafMisc.h"

class Board;

class Scoreboard
{
public:
	Scoreboard(Board* board);
    void Update(uint millis);
    void IncHome();
    void IncAway();
    void Reset();
    void Draw();
    void SetCountDown(uint count){ _countDown = count; }
    int GetCurrentCountDown() const { return _currentCountDown; }
private:
    int _countDown;
    uint _lastCountDown;
    uint _home;
    uint _away;
    uint _firstMillis;
    std::string _currentTime;
    int _currentCountDown;
	Board* _board;
};

#endif
