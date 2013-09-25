#ifndef YafAppearance_h__
#define YafAppearance_h__

#include <string>
#include <CGFtexture.h>
#include "YafMisc.h"

class YafTexture : public YafElement, public CGFtexture
{
public:
    YafTexture(const std::string& id, const std::string& file) : YafElement(id), CGFtexture(file) { }
    std::string File;
};

class YafAppearance : public YafElement
{
public:
    YafAppearance(const std::string& id) : YafElement(id) { }
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
