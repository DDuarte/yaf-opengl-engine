#ifndef YafLight_h__
#define YafLight_h__

#include <CGFlight.h>
#include "YafMisc.h"

class YafLight : public YafElement, public CGFlight
{
public:
    YafLight(const std::string& id, int i, YafXYZW<> pos) : YafElement(id), CGFlight(GL_LIGHT0 + i, pos) { }
    YafLight(const std::string& id, int i, YafXYZW<> pos, YafXYZ<> dir) : YafElement(id), CGFlight(GL_LIGHT0 + i, pos, dir) { }
    bool Enabled;
    YafXYZW<> Ambient;
    YafXYZW<> Diffuse;
    YafXYZW<> Specular;

    virtual void InitLight()
    {
        setAmbient(Ambient);
        setDiffuse(Diffuse);
        setSpecular(Specular);
    }
};

class YafOmniLight : public YafLight
{
public:
    YafOmniLight(const std::string& id, int i, YafXYZW<> pos) : YafLight(id, i, pos)  { }
};

class YafSpotLight : public YafLight
{
public:
    YafSpotLight(const std::string& id, int i, YafXYZW<> pos, YafXYZ<> dir) : YafLight(id, i, pos, dir) { }

    virtual void InitLight()
    {
        YafLight::InitLight();
        setAngle(Angle);
        glLightf(id, GL_SPOT_EXPONENT, Exponent);
    }

    float Angle;
    float Exponent;
};

#endif // YafLight_h__
