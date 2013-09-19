#ifndef YafLight_h__
#define YafLight_h__

class YafLight : public YafElement
{
public:
    bool Enabled;
    YafXYZ Location;
    YafRGBA Ambient;
    YafRGBA Diffuse;
    YafRGBA Specular;
};

class YafOmniLight : public YafLight
{

};

class YafSpotLight : public YafLight
{
public:
    float Angle;
    float Exponent;
    YafXYZ Direction;
};

#endif // YafLight_h__
