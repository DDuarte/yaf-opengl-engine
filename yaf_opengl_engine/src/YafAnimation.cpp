#include "YafAnimation.h"

#define _USE_MATH_DEFINES // for M_PI
#include <math.h>

#include <iostream>

void YafLinearAnimation::ApplyAnimation()
{
    glTranslatef(_currentPoint.X, _currentPoint.Y, _currentPoint.Z);
    glRotatef(_currentAngle, 0.0f, 1.0f, 0.0f);
}

void YafPlanetAnimation::ApplyAnimation()
{
    glRotatef(_translateAngle, 0.0f, 1.0f, 0.0f);
    glTranslatef(_position.X, _position.Y, _position.Z);
    glRotatef(_rotationAngle, 0.0f, 1.0f, 0.0f);
}

void YafPieceAnimation::ApplyAnimation()
  {
    glTranslatef(Animation->getCurrentPoint().X, Animation->getCurrentPoint().Y, 0.0f);
  }

int YafLinearAnimation::Position(unsigned long diff, float& path)
{
    float distance = _speed * diff;
    for(int i = 0 ; i < _controlPointsDistance.size() ; ++i)
    {
        float compare = 0;
        for (int j = i ; j >= 0 ; --j)
            compare += _controlPointsDistance[j];
        if (distance <= compare)
        {
            path = _controlPointsDistance[i] - (compare - distance);
            return i;
        }
    }
    return static_cast<int>(_controlPointsDistance.size());
}

void YafLinearAnimation::Update(unsigned long millis)
{
    if (_firstMillis == 0)
        _firstMillis = millis;

    unsigned long diff = millis - _firstMillis;
    float path;
    int position = Position(diff, path);

    _currentPoint = _controlPoints[position];            // ponto inicial do troço

    if (position != _controlPoints.size() - 1)
    {
        YafXYZ vector(_controlPoints[position + 1].X - _controlPoints[position].X,
            _controlPoints[position + 1].Y - _controlPoints[position].Y,
            _controlPoints[position + 1].Z - _controlPoints[position].Z);

        vector = vector.GetNormalized();

        _currentPoint.X += vector.X  * path;
        _currentPoint.Y += vector.Y  * path;
        _currentPoint.Z += vector.Z  * path;

        if (vector.Z != 0.0f)
        {
            _currentAngle = atan(vector.X / vector.Z) * 180.0f / static_cast<float>(M_PI);

            if (_currentAngle == 0.0f && vector.Z < 0.0f)
                _currentAngle = 180.0f;
        }
        else if (vector.X != 0.0f)
            _currentAngle = 90.0f * vector.X / abs(vector.X);
    }
}

YafLinearAnimation::YafLinearAnimation(const std::string& id, float time, const std::vector<YafXYZ>& controlPoints) : YafAnimation(id), _time(static_cast<unsigned long>(time * 1000)), _controlPoints(controlPoints), _firstMillis(0), _currentPoint(controlPoints[0]), _currentAngle(0)
{
    float distance = 0;
    for (int i = 0; i < _controlPoints.size() - 1; ++i)
    {
        distance += YafXYZ::GetDistance(_controlPoints[i], _controlPoints[i + 1]);
        _controlPointsDistance.push_back(YafXYZ::GetDistance(_controlPoints[i], _controlPoints[i + 1]));
    }
    _speed = distance / _time;
}

void YafPlanetAnimation::Update(unsigned long millis)
{
    if (_firstMillis == 0)
        _firstMillis = millis;

    unsigned long diff = millis - _firstMillis;

    _rotationAngle = 360.0f * diff / static_cast<float>(_rTime);
    _translateAngle = 360.0f * diff / static_cast<float>(_tTime);
}

YafPlanetAnimation::YafPlanetAnimation(const std::string& id, float rtime, float ttime, YafXYZ position) : YafAnimation(id), _rTime(static_cast<unsigned long>(rtime) * 1000u),
_tTime(static_cast<unsigned long>(ttime) * 1000u), _firstMillis(0u), _rotationAngle(0.0f), _translateAngle(0.0f), _position(position)
{

}
  
YafPieceAnimation::YafPieceAnimation(const std::string& id, YafXYZ position) : YafAnimation(id)
{
    std::vector<YafXYZ> points;
    points.push_back(position);
    points.push_back(position);
    Animation = new YafLinearAnimation(id, 5, points);
}

void YafPieceAnimation::Update(unsigned long millis)
{
    Animation->Update(millis);
}

void YafPieceAnimation::moveTo(unsigned int X, unsigned int Y)
{
    if(X <= 7 && X >= 0 && Y <= 6 && Y >= 0)
    {
        YafXYZ moveTo; 
        moveTo.Z = 0;
        moveTo.Y = (6 - Y) * -2.35;
        if(X == 0)
            Y *= 2;
        if(Y % 2 == 0)
             moveTo.X = (7 - X) * 2.7;
        else
           moveTo.X = (7 - (X-1)) * 2.7 - 1.35;
        std::vector<YafXYZ> points;
        points.push_back(Animation->getCurrentPoint());
        points.push_back(moveTo);
        Animation = new YafLinearAnimation(Id, 5, points);
    }
}