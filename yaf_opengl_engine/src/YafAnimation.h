#ifndef ANIMATION_H_
#define ANIMATION_H_

#include "YafMisc.h"
#include "YafNode.h"

class YafAnimation : public YafElement
{
public:
    YafAnimation(const std::string& id, YafNode* node) : YafElement(id), Node(node) { }
    virtual void ApplyAnimation() = 0;
    virtual void Update(unsigned long millis) = 0;
protected:
    YafNode* Node;
};

class YafLinearAnimation : public YafAnimation
{
public:
    virtual void ApplyAnimation() override;
    YafLinearAnimation(const std::string& id, YafNode* node, float time, const std::vector<YafXYZ>& controlPoints);
    virtual void Update(unsigned long millis) override;
    YafXYZ GetCurrentPoint() const { return _currentPoint; }
private:
    int Position(unsigned long millis, float& path);
    unsigned long _time;
    std::vector<YafXYZ> _controlPoints;
    unsigned long _firstMillis;
    YafXYZ _currentPoint;
    float _speed;
    std::vector<float> _controlPointsDistance;
    float _currentAngle;
};

class YafPieceAnimation : public YafAnimation
{
public:
    YafPieceAnimation(const std::string& id, YafNode* node, int x1, int y1, int x2, int y2);
    ~YafPieceAnimation();
    virtual void ApplyAnimation() override;
    virtual void Update(unsigned long millis) override;
private:
    YafLinearAnimation* _animation;
    void MoveTo(int x1, int y1, int x2, int y2);

    static YafXY BoardIndexesToXY(int xi, int yi);
};

#endif
