#include "YafAnimation.h"

#define _USE_MATH_DEFINES // for M_PI
#define SPEED 5.0f
#include <math.h>
#include <cassert>
#include <iostream>

void YafLinearAnimation::ApplyAnimation() 
{
    Node->Position = _currentPoint;
    Node->Yaw = _currentAngle;
}

void YafPieceAnimation::ApplyAnimation()
{
    Node->Position.X = _animation->GetCurrentPoint().X;
    Node->Position.Y = _animation->GetCurrentPoint().Y;
    // Node->Position.Z = _animation->GetCurrentPoint().Z;
}

int YafLinearAnimation::Position(unsigned long diff, float& path)
{
    float distance = _speed * diff;
    for(int i = 0; i < _controlPointsDistance.size(); ++i)
    {
        float compare = 0;
        for (int j = i; j >= 0; --j)
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

YafLinearAnimation::YafLinearAnimation(const std::string& id, YafNode* node, float time, const std::vector<YafXYZ>& controlPoints) : YafAnimation(id, node), _time(static_cast<unsigned long>(time * 1000)), _controlPoints(controlPoints), _firstMillis(0), _currentPoint(controlPoints[0]), _currentAngle(0)
{
    float distance = 0;
    for (int i = 0; i < _controlPoints.size() - 1; ++i)
    {
        distance += YafXYZ::GetDistance(_controlPoints[i], _controlPoints[i + 1]);
        _controlPointsDistance.push_back(YafXYZ::GetDistance(_controlPoints[i], _controlPoints[i + 1]));
    }
    _speed = distance / _time;
}

YafPieceAnimation::YafPieceAnimation(const std::string& id, YafNode* node, int x1, int y1, int x2, int y2) : YafAnimation(id, node)
{
    MoveTo(x1, y1, x2, y2);
}

void YafPieceAnimation::Update(unsigned long millis)
{
    _animation->Update(millis);
}

void YafPieceAnimation::MoveTo(int x1, int y1, int x2, int y2)
{
    assert(x1 <= 7 && x1 >= 0 && y1 <= 6 && y1 >= 0 && "Invalid src coordinates in MoveTo");
    assert(x2 <= 7 && x2 >= 0 && y2 <= 6 && y2 >= 0 && "Invalid dest coordinates in MoveTo");

    auto moveFrom = BoardIndexesToXY(x1, y1);
    auto moveTo = BoardIndexesToXY(x2, y2);

    std::vector<YafXYZ> points(2);
    points[0] = YafXYZ(moveFrom.X, moveFrom.Y, 0.0f);
    points[1] = YafXYZ(moveTo.X, moveTo.Y, 0.0f);
    auto dist = sqrt(pow(moveTo.X - moveFrom.X, 2.0f) + pow(moveTo.Y - moveFrom.Y, 2.0f));
    _animation = new YafLinearAnimation(Id + "In", Node, dist / SPEED, points);
}

YafPieceAnimation::~YafPieceAnimation()
{
    delete _animation;
}

YafXY YafPieceAnimation::BoardIndexesToXY(int xi, int yi)
{
    if (xi == 0)
        yi *= 2;

    YafXY move;
    move.Y = (6.0f - yi) * -2.35f;

    if (yi % 2 == 0)
        move.X = (7.0f - xi) * 2.7f;
    else
        move.X = (7.0f - (xi - 1.0f)) * 2.7f - 1.35f;

    move.X += -9.45f;
    move.Y += 7.05f;

    return move;
}
