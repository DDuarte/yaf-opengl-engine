#ifndef YafMisc_h__
#define YafMisc_h__

#include <string>
#include <stdexcept>
#include <vector>

class YafParsingException : public std::runtime_error
{
public:
    YafParsingException(const std::string& what) : runtime_error(what) { }
};

class YafElement // anything with an id
{
public:
    YafElement(const std::string& id) : Id(id) { }
    std::string Id;
};

class YafXY
{
public:
    YafXY() : X(0.0f), Y(0.0f) { }
    YafXY(const std::string& s);

    operator float*()
    {
        return reinterpret_cast<float*>(this); // this won't work if class has virtual methods
    }

    float X;
    float Y;
};

class YafXYZ
{
public:
    YafXYZ() : X(0.0f), Y(0.0f), Z(0.0f) { }
    YafXYZ(float x, float y, float z) : X(x), Y(y), Z(z) { }
    YafXYZ(const std::string& s);

    operator float*()
    {
        return reinterpret_cast<float*>(this); // this won't work if class has virtual methods
    }

    float X;
    float Y;
    float Z;
    YafXYZ GetNormalized();
};

class YafRGBA
{
public:
    YafRGBA() : R(0.0f), G(0.0f), B(0.0f), A(0.0f) { }
    YafRGBA(const std::string& s);

    operator float*()
    {
        return reinterpret_cast<float*>(this); // this won't work if class has virtual methods
    }

    float R;
    float G;
    float B;
    float A;
};

#endif // YafMisc_h__
