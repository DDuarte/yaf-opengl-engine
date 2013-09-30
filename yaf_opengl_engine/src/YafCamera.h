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
    YafPerspectiveCamera(const std::string& id) : YafCamera(id) { }
    float Angle;
    YafXYZ Position;
    YafXYZ Target;

    virtual void applyView() override
    {
        /*glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        gluLookAt(Position.X, Position.Y, Position.Z,
                  Target.X, Target.Y, Target.Z,
                  0.0, 1.0, 0.0);*/

        glTranslatef(position[0], position[1], position[2]);

        glRotatef(rotation[0], 1.f, 0.f, 0.f);
        glRotatef(rotation[1], 0.f, 1.f, 0.f);
        glRotatef(rotation[2], 0.f, 0.f, 1.f);
    }

    virtual void updateProjectionMatrix(int width, int height) override
    {
        double aspect = (double) width / (double) height;
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        gluPerspective(Angle, aspect, Near, Far);
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
        glOrtho(Left, Right, Bottom, Top, Near, Far);
    }

    virtual void updateProjectionMatrix(int /*width*/, int /*height*/) override
    {
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        gluOrtho2D(Left, Right, Bottom, Top);
    }
};

#endif // YafCamera_h__
