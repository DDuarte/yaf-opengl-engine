#ifndef YafTransform_h__
#define YafTransform_h__

#include "YafEnums.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class YafTransform
{
public:
    virtual void ApplyTransform(glm::mat4& m) = 0;
};

class YafTranslateTransform : public YafTransform
{
public:
    YafXYZ To;

    virtual void ApplyTransform(glm::mat4& m) override
    {
        m = glm::translate(m, glm::vec3(To.X, To.Y, To.Z));
    }
};

class YafRotateTransform : public YafTransform
{
public:
    YafAxis Axis;
    float Angle;

    virtual void ApplyTransform(glm::mat4& m) override
    {
        glm::vec3 axis;

        switch (Axis)
        {
        case X:
            axis = glm::vec3(1.0f, 0.0f, 0.0f);
            break;
        case Y:
            axis = glm::vec3(0.0f, 1.0f, 0.0f);
            break;
        case Z:
            axis = glm::vec3(0.0f, 0.0f, 1.0f);
            break;
        }

        m = glm::rotate(m, Angle, axis);
    }
};

class YafScaleTransform : public YafTransform
{
public:
    YafXYZ Factor;

    virtual void ApplyTransform(glm::mat4& m) override
    {
        m = glm::scale(m, glm::vec3(Factor.X, Factor.Y, Factor.Z));
    }
};

#endif // YafTransform_h__
