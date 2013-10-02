#ifndef YafTransform_h__
#define YafTransform_h__

#include "YafEnums.h"

class YafTransform
{
public:
    virtual void ApplyTransform() = 0;
};

class YafTranslateTransform : public YafTransform
{
public:
    YafXYZ To;

    virtual void ApplyTransform() override
    {
        glTranslatef(To.X, To.Y, To.Z);
    }
};

class YafRotateTransform : public YafTransform
{
public:
    YafAxis Axis;
    float Angle;

    virtual void ApplyTransform() override
    {
        switch (Axis)
        {
        case X:
            glRotatef(Angle, 1.0f, 0.0f, 0.0f);
            break;
        case Y:
            glRotatef(Angle, 0.0f, 1.0f, 0.0f);
            break;
        case Z:
            glRotatef(Angle, 0.0f, 0.0f, 1.0f);
            break;
        }
    }
};

class YafScaleTransform : public YafTransform
{
public:
    YafXYZ Factor;

    virtual void ApplyTransform() override
    {
        glScalef(Factor.X, Factor.Y, Factor.Z);
    }
};

#endif // YafTransform_h__
