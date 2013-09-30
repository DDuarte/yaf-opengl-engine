#ifndef YafAppearance_h__
#define YafAppearance_h__

#include <string>
#include <CGFtexture.h>
#include <CGFappearance.h>
#include "YafMisc.h"

class YafTexture : public YafElement, public CGFtexture
{
public:
    YafTexture(const std::string& id, const std::string& file) : YafElement(id), CGFtexture(), File(file) { }

    std::string File;

    void InitTexture()
    {
        loadTexture(File);
    }
};

class YafAppearance : public YafElement, public CGFappearance
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

    void InitAppearance()
    {
        setEmissive(Emissive);
        setAmbient(Ambient);
        setDiffuse(Diffuse);
        setSpecular(Specular);
        setShininess(Shininess);

        if (Texture)
            setTexture(Texture);
    }
};

#endif // YafAppearance_h__
