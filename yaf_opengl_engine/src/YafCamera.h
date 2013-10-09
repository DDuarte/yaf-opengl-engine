#ifndef YafCamera_h__
#define YafCamera_h__

#include "YafMisc.h"
#include <CGFcamera.h>
#include <gl/GL.h>

class YafCamera : public YafElement, public CGFcamera
{
public:
    YafCamera(const std::string& id) : YafElement(id), CGFcamera() { }
    float Near;
    float Far;
};

class YafPerspectiveCamera : public YafCamera
{
public:
    YafPerspectiveCamera(const std::string& id, YafXYZ pos) : YafCamera(id)
    {
        position[0] = pos.X;
        position[1] = pos.Y;
        position[2] = pos.Z;
    }

    float Angle;
    YafXYZ Target;

    virtual void applyView() override
    {
        glMatrixMode(GL_MODELVIEW);

        glLoadIdentity();
        gluLookAt(position[0], position[1], position[2],
            Target.X, Target.Y, Target.Z,
            0.0, 1.0, 0.0);
    }

    virtual void updateProjectionMatrix(int width, int height) override
    {
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        gluPerspective(Angle, (double) width / (double) height, Near, Far);
    }
};

class YafOrthoCamera : public YafCamera
{
public:
    YafOrthoCamera(const std::string& id) : YafCamera(id) { }
    float Left;
    float Right;
    float Top;
    float Bottom;

    virtual void applyView() override
    {
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
    }

    virtual void updateProjectionMatrix(int /*width*/, int /*height*/) override
    {
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glOrtho(Left, Right, Bottom, Top, Near, Far);
    }
};

#endif // YafCamera_h__
