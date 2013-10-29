#ifndef ANIMATION_H_
#define ANIMATION_H_

#include "YafMisc.h"
#include <windows.h>
#include <gl/GL.h>
#include <gl/freeglut.h>
#include <gl/glui.h>

class YafAnimation
{
public:
    virtual void ApplyAnimation() = 0;
    virtual void Update(unsigned long millis) = 0;
};

class YafLinearAnimation : public YafAnimation
{
public:
    virtual void ApplyAnimation() override;
    YafLinearAnimation() : _time(0), _firstMillis(0) { }
    YafLinearAnimation(unsigned long time, const std::vector<YafXYZ>& controlPoints) : _time(time), _controlPoints(controlPoints), _firstMillis(0), _currentPoint(controlPoints[0])
    {
        float distance = 0;
        for(int i = 0 ; i < _controlPoints.size() - 1 ; ++i){
            distance += YafXYZ::GetDistance(_controlPoints[i], _controlPoints[i + 1]);
            _controlPointsDistance.push_back(YafXYZ::GetDistance(_controlPoints[i], _controlPoints[i + 1]));
        }
        _speed = distance / _time;
    }
    virtual void Update(unsigned long millis) override;
    int Position(unsigned long millis, float& path);
private:
    unsigned long _time;
    std::vector<YafXYZ> _controlPoints;
    unsigned long _firstMillis;
    YafXYZ _currentPoint;
    float _speed;
    std::vector<float> _controlPointsDistance;
};

#endif
