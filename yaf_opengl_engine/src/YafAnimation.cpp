#include "YafAnimation.h"

void YafLinearAnimation::ApplyAnimation()
{
    glTranslatef(_currentPoint.X, _currentPoint.Y, _currentPoint.Z);

}

int YafLinearAnimation::Position(unsigned long diff, float& path)
{
    float distance = _speed * diff;

    for(int i = 0 ; i < _controlPointsDistance.size() ; ++i)
    {
        float compare = 0;
        for(int j = i ; j >= 0 ; --j)
            compare += _controlPointsDistance[j];
        if( distance <= compare)
        {
            path = _controlPointsDistance[i] - (compare - distance);
            return i;
        }
    }
    return -1;
}

void YafLinearAnimation::Update(unsigned long millis)
{
    if(_firstMillis == 0)
        _firstMillis = millis;

    unsigned long diff = millis - _firstMillis;
    float path;
    int position = Position(diff, path);

    _currentPoint = _controlPoints[position];            // ponto inicial do troço


    YafXYZ vector(_controlPoints[position + 1].X - _controlPoints[position].X,
                 _controlPoints[position + 1].Y - _controlPoints[position].Y,
                 _controlPoints[position + 1].Z - _controlPoints[position].Z);

    vector = vector.GetNormalized();

    _currentPoint.X += vector.X  * path;
    _currentPoint.Y += vector.Y  * path;
    _currentPoint.Z += vector.Z  * path;

}