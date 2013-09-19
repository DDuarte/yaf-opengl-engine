#ifndef YafAppearance_h__
#define YafAppearance_h__

#include <string>
#include "YafMisc.h"

class YafTexture : public YafElement
{
public:
    std::string File;
};

class YafAppearance : public YafElement
{
public:
    YafRGBA Emissive;
    YafRGBA Ambient;
    YafRGBA Diffuse;
    YafRGBA Specular;
    float Shininess;
    YafTexture* Texture; // can be null
    float TexLengthS;
    float TexLengthT;
};

#endif // YafAppearance_h__
