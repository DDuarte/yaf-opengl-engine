#ifndef YafTransform_h__
#define YafTransform_h__

#include "YafEnums.h"

class YafTransform
{

};

class YafTranslateTransform : public YafTransform
{
public:
    YafXYZ To;
};

class YafRotateTransform : public YafTransform
{
public:
    YafAxis Axis;
    float Angle;
};

class YafScaleTransform : public YafTransform
{
public:
    YafXYZ Factor;
};

#endif // YafTransform_h__
