#ifndef ANIMATION_H_
#define ANIMATION_H_

#include "YafMisc.h"
#include "YafNode.h"

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
    YafLinearAnimation(const std::string& id, float time, const std::vector<YafXYZ>& controlPoints);
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
    YafPlanetAnimation(const std::string& id, float rtime, float ttime, YafXYZ position);
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
