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
            _currentAngle = atan(vector.X / vector.Z) * 180.0f / M_PI;

            if (_currentAngle == 0.0f && vector.Z < 0.0f)
                _currentAngle = 180.0f;
        }
        else if (vector.X != 0.0f)
            _currentAngle = 90.0f * vector.X / abs(vector.X);
    }
}

void YafPlanetAnimation::Update(unsigned long millis)
{
    if(_firstMillis == 0)
        _firstMillis = millis;

    unsigned long diff = millis - _firstMillis;
    
  _rotationAngle = 360  * diff / _rTime;
  _translateAngle = 360  * diff / _tTime;

}