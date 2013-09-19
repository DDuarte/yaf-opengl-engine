#ifndef YafCamera_h__
#define YafCamera_h__

#include "YafMisc.h"

class YafCamera : public YafElement
{
public:
    float Near;
    float Far;
};

class YafPerspectiveCamera : public YafCamera
{
public:
    float Angle;
    YafXYZ Position;
    YafXYZ Target;
};

class YafOrthoCamera : public YafCamera
{
public:
    float Left;
    float Right;
    float Top;
    float Bottom;
};

#endif // YafCamera_h__
