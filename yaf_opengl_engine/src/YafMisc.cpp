#include "YafMisc.h"
#include "Utilities.h"

YafXY::YafXY(const std::string& s)
{
    std::vector<float> XY = ParseFloats(s, 2);

    X = XY[0];
    Y = XY[1];
}

YafXYZ::YafXYZ(const std::string& s)
{
    std::vector<float> XYZ = ParseFloats(s, 3);

    X = XYZ[0];
    Y = XYZ[1];
    Z = XYZ[2];
}

YafXYZ YafXYZ::GetNormalized()
{
    float length = sqrt(X * X + Y * Y + Z * Z);
    return YafXYZ(X / length, Y / length, Z / length);
}

float YafXYZ::GetDistance(const YafXYZ& p1, const YafXYZ& p2)
{
    return sqrt(pow(p2.X - p1.X, 2.0f) + pow(p2.Y - p1.Y, 2.0f) + pow(p2.Z - p1.Z, 2.0f));
}

YafRGBA::YafRGBA(const std::string& s)
{
    std::vector<float> RGBA = ParseFloats(s, 4);

    R = RGBA[0];
    G = RGBA[1];
    B = RGBA[2];
    A = RGBA[3];
}

YafXYZW::YafXYZW(const std::string& s)
{
    std::vector<float> XYZW = ParseFloats(s, 4);

    X = XYZW[0];
    Y = XYZW[1];
    Z = XYZW[2];
    W = XYZW[3];
}
