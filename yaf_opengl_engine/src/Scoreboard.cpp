#include "Scoreboard.h"
#include "Game.h"

#include <gl/glew.h>
#include <gl/freeglut.h>


Scoreboard::Scoreboard(Board* board)
{
    ResetScores();
    ResetCountdown();
    ResetTimer();
    _board = board;
}

void Scoreboard::Update(uint millis)
{
   if (_firstMillis == 0)
       _firstMillis = millis;

   if (!_currentTime.empty() && std::to_string(millis / 1000).back() == _currentTime.back())
       return;

   uint currentTime = (millis - _firstMillis) / 1000;
   uint minutes = currentTime / 60;
   uint seconds = currentTime - minutes * 60;
   std::string strMinutes = std::to_string(minutes), strSeconds = std::to_string(seconds);

   if (minutes < 10)
       strMinutes = '0' + strMinutes;
   if (seconds < 10)
       strSeconds = '0' + strSeconds;

   _currentTime = strMinutes + ':' + strSeconds;

   if (_countDown == -1)
   {
       _lastCountDown = millis / 1000;
       return;
   }

   else if (_reset)
   {
       _lastCountDown = millis / 1000;
       _reset = false;
       return;
   }

   _currentCountDown = _countDown - (millis/1000 - _lastCountDown);

   if (_currentCountDown < 0)
   {
       _countDown = -1;
       if (_board->GetCurrentPlayer() == Player::First)
           _away++;
       else if (_board->GetCurrentPlayer() == Player::Second)
           _home++;
   }
}

void Scoreboard::IncHome()
{
    _home++;
}
void Scoreboard::IncAway()
{
    _away++;
}

void Scoreboard::Draw()
{
    std::string home = std::to_string(_home), away = std::to_string(_away);
    glDisable(GL_LIGHTING);

    // HOME
    glPushMatrix();
        glTranslatef(13.1f, 2.0f, -7.5f);
        glScalef(0.02f, 0.02f, 0.02f);
        glRotatef(90.0f, 0.0f, -1.0f, 0.0f);
        glColor3f(1.0f, 1.0f, 1.0f);        // branco
        if (home.size() == 1)
            home = "0" + home;

        for (uint i = 0 ; i < home.size() ; ++i)
            glutStrokeCharacter(GLUT_STROKE_ROMAN, home[i]);
    glPopMatrix();

    // AWAY
    glPushMatrix();
        glTranslatef(13.1f, 2.0f, 4.5f);
        glScalef(0.02f, 0.02f, 0.02f);
        glRotatef(90.0f, 0.0f, -1.0f, 0.0f);
        glColor3f(1.0f, 1.0f, 1.0f);        // branco
        if (away.size() == 1)
            away = "0" + away;
        for (uint i = 0 ; i < away.size() ; ++i)
            glutStrokeCharacter(GLUT_STROKE_ROMAN, away[i]);
    glPopMatrix();

    // TIME
    glPushMatrix();
        glTranslatef(13.1f, 2.0f, -3.25f);
        glScalef(0.02f, 0.02f, 0.02f);
        glRotatef(90.0f, 0.0f, -1.0f, 0.0f);
        glColor3f(1.0f, 1.0f, 1.0f);        // branco

        for (uint i = 0; i < _currentTime.size(); ++i)
            glutStrokeCharacter(GLUT_STROKE_ROMAN, _currentTime[i]);
    glPopMatrix();

    // COUNTDOWN
    if (_countDown != -1)
    {
        glPushMatrix();
            glTranslatef(13.1f, 5.53f, -1.5f);
            glScalef(0.02f, 0.02f, 0.02f);
            glRotatef(90.0f, 0.0f, -1.0f, 0.0f);
            glColor3f(1.0f, 1.0f, 1.0f);        // branco

            std::string countDownStr;

            if(_currentCountDown < 10)
                countDownStr = "0";

            countDownStr += std::to_string(_currentCountDown);
            for (uint i = 0 ; i < countDownStr.size(); ++i)
                glutStrokeCharacter(GLUT_STROKE_ROMAN, countDownStr[i]);
        glPopMatrix();
    }

    glEnable(GL_LIGHTING);
}

void Scoreboard::ResetCountdown()
{
    _countDown = COUNTDOWN;
    _reset = true;
}
