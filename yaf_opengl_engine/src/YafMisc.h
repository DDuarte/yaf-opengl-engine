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
    std::string Id;
};

class YafXY
{
public:
    YafXY() : X(0.0f), Y(0.0f) { }

    YafXY(const std::string& s);

    float X;
    float Y;
};

class YafXYZ
{
public:
    YafXYZ() : X(0.0f), Y(0.0f), Z(0.0f) { }

    YafXYZ(const std::string& s);

    float X;
    float Y;
    float Z;
};

class YafRGBA
{
public:
    YafRGBA() : R(0.0f), G(0.0f), B(0.0f), A(0.0f) { }

    YafRGBA(const std::string& s);

    float R;
    float G;
    float B;
    float A;
};

#endif // YafMisc_h__
