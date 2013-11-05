#ifndef ANIMATION_H_
#define ANIMATION_H_

#include "YafMisc.h"
#include "YafNode.h"
#include <windows.h>
#include <gl/glew.h>
#include <gl/freeglut.h>
#include <gl/glui.h>

class YafAnimation : public YafElement
{
public:
    YafAnimation(const std::string& id) : YafElement(id) { }
    virtual void ApplyAnimation() = 0;
    virtual void Update(unsigned long millis) = 0;
};

class YafLinearAnimation : public YafAnimation
{
public:
    virtual void ApplyAnimation() override;
    YafLinearAnimation(const std::string& id, float time, const std::vector<YafXYZ>& controlPoints) : YafAnimation(id), _time(static_cast<unsigned long>(time * 1000)), _controlPoints(controlPoints), _firstMillis(0), _currentPoint(controlPoints[0]), _currentAngle(0)
    {
        float distance = 0;
        for (int i = 0; i < _controlPoints.size() - 1; ++i)
        {
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
    float _currentAngle;
};

class YafPlanetAnimation : public YafAnimation
{
public:
    virtual void ApplyAnimation() override;
    YafPlanetAnimation(const std::string& id, float rtime, float ttime, YafXYZ position) : YafAnimation(id), _rTime(rtime * 1000),
    _tTime(ttime * 1000), _firstMillis(0), _rotationAngle(0), _translateAngle(0), _position(position) { }
    virtual void Update(unsigned long millis) override;
private:
    unsigned long _firstMillis;
    YafXYZ _position;
    unsigned long _rTime;
    unsigned long _tTime;
    float _rotationAngle;
    float _translateAngle;
};

#endif
