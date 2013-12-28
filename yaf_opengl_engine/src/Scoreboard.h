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
    void ResetScores() { _home = 0; _away = 0; }
    void ResetCountdown() { _countDown = 30; _lastCountDown = 0; _currentCountDown = 0; }
    void ResetTimer() { _firstMillis = 0; }
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
