#ifndef YafMisc_h__
#define YafMisc_h__

#include <string>
#include <stdexcept>
#include <vector>

#include "Utilities.h"

typedef unsigned int uint;

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

template<typename T = float>
class YafXY
{
public:
    YafXY() : X(static_cast<T>(0)), Y(static_cast<T>(0)) { }
    YafXY(T x, T y) : X(x), Y(y) { }
    YafXY(const std::string& s)
    {
        std::vector<float> XY = ParseNumbers<2>(s);

        X = XY[0];
        Y = XY[1];
    }

    operator T*()
    {
        return reinterpret_cast<T*>(this); // this won't work if class has virtual methods
    }

    T X;
    T Y;
};

template<typename T = float>
class YafXYZ
{
public:
    YafXYZ() : X(static_cast<T>(0)), Y(static_cast<T>(0)), Z(static_cast<T>(0)) { }
    YafXYZ(T x, T y, T z) : X(x), Y(y), Z(z) { }
    YafXYZ(const std::string& s)
    {
        std::vector<float> XYZ = ParseNumbers<3>(s);

        X = XYZ[0];
        Y = XYZ[1];
        Z = XYZ[2];
    }

    operator T*()
    {
        return reinterpret_cast<T*>(this); // this won't work if class has virtual methods
    }

    YafXYZ& operator +=(const YafXYZ& other)
    {
        X += other.X;
        Y += other.Y;
        Z += other.Z;
        return *this;
    }

    T X;
    T Y;
    T Z;

    YafXYZ<> GetNormalized()
    {
        T length = sqrt(X * X + Y * Y + Z * Z);
        return YafXYZ(X / length, Y / length, Z / length);
    }

    static T GetDistance(const YafXYZ& p1, const YafXYZ& p2)
    {
        return static_cast<T>(sqrt(pow(p2.X - p1.X, static_cast<T>(2)) + pow(p2.Y - p1.Y, static_cast<T>(2)) + pow(p2.Z - p1.Z, static_cast<T>(2))));
    }
};

template<typename T = float>
class YafXYZW
{
public:
    YafXYZW() : X(static_cast<T>(0)), Y(static_cast<T>(0)), Z(static_cast<T>(0)), W(static_cast<T>(0)) { }
    YafXYZW(T x, T y, T z, T w) : X(x), Y(y), Z(z), W(w) { }
    YafXYZW(const YafXYZ<T>& p, T w) : X(p.X), Y(p.Y), Z(p.Z), W(w) { }
    YafXYZW(const std::string& s)
    {
        std::vector<float> XYZW = ParseNumbers<4>(s);

        X = XYZW[0];
        Y = XYZW[1];
        Z = XYZW[2];
        W = XYZW[3];
    }

    operator T*()
    {
        return reinterpret_cast<T*>(this); // this won't work if class has virtual methods
    }

    T X;
    T Y;
    T Z;
    T W;
};

#endif // YafMisc_h__
