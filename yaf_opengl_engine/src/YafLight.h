#ifndef YafLight_h__
#define YafLight_h__

class YafLight : public YafElement
{
public:
    YafLight(const std::string& id) : YafElement(id) { }
    bool Enabled;
    YafXYZ Location;
    YafRGBA Ambient;
    YafRGBA Diffuse;
    YafRGBA Specular;
};

class YafOmniLight : public YafLight
{
public:
    YafOmniLight(const std::string& id) : YafLight(id) { }
};

class YafSpotLight : public YafLight
{
public:
    YafSpotLight(const std::string& id) : YafLight(id) { }
    float Angle;
    float Exponent;
    YafXYZ Direction;
};

#endif // YafLight_h__
