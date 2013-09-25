#ifndef YafLight_h__
#define YafLight_h__

#include <CGFlight.h>

class YafLight : public YafElement, public CGFlight
{
public:
    YafLight(const std::string& id, int i, YafXYZ pos) : YafElement(id), Location(pos), CGFlight(0x4000 + i, reinterpret_cast<float*>(&pos)) { }
    YafLight(const std::string& id, int i, YafXYZ pos, YafXYZ dir) : YafElement(id), Location(pos), CGFlight(0x4000 + i, reinterpret_cast<float*>(&pos), reinterpret_cast<float*>(&dir)) { }
    bool Enabled;
    YafXYZ Location;
    YafRGBA Ambient;
    YafRGBA Diffuse;
    YafRGBA Specular;
    virtual void UpdateLight()
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

    virtual void UpdateLight() 
    {
        YafLight::UpdateLight();
        setAngle(Angle);
        glLightf(id, GL_SPOT_EXPONENT, Exponent);
        glLightfv(id, GL_SPOT_DIRECTION, reinterpret_cast<float*>(&Direction));
    }

    float Angle;
    float Exponent;
    YafXYZ Direction;
};

#endif // YafLight_h__
