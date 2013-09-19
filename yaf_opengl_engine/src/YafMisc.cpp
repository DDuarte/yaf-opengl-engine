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

YafRGBA::YafRGBA(const std::string& s)
{
    std::vector<float> RGBA = ParseFloats(s, 4);

    R = RGBA[0];
    G = RGBA[1];
    B = RGBA[2];
    A = RGBA[3];
}
