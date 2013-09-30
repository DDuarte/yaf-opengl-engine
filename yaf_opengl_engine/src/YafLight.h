#ifndef YafLight_h__
#define YafLight_h__

#include <CGFlight.h>
#include "YafMisc.h"

class YafLight : public YafElement, public CGFlight
{
public:
    YafLight(const std::string& id, int i, YafXYZ pos) : YafElement(id), Location(pos), CGFlight(0x4000 + i, pos) { }
    YafLight(const std::string& id, int i, YafXYZ pos, YafXYZ dir) : YafElement(id), Location(pos), CGFlight(0x4000 + i, pos, dir) { }
    bool Enabled;
    YafXYZ Location;
    YafRGBA Ambient;
    YafRGBA Diffuse;
    YafRGBA Specular;

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
    YafOmniLight(const std::string& id, int i, YafXYZ pos) : YafLight(id, i, pos)  { }
};

class YafSpotLight : public YafLight
{
public:
    YafSpotLight(const std::string& id, int i, YafXYZ pos, YafXYZ dir) : YafLight(id, i, pos, dir), Direction(dir) { }

    virtual void InitLight()
    {
        YafLight::InitLight();
        setAngle(Angle);
        glLightf(id, GL_SPOT_EXPONENT, Exponent);
        glLightfv(id, GL_SPOT_DIRECTION, Direction);
    }

    float Angle;
    float Exponent;
    YafXYZ Direction;
};

#endif // YafLight_h__
