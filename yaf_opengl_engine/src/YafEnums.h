#ifndef YafEnums_h__
#define YafEnums_h__

#include <string>
#include <windows.h>
#include <gl/GL.h>

#include "YafMisc.h"

enum YafAxis
{
    X,
    Y,
    Z
};

inline YafAxis YafAxisFromString(const std::string& str)
{
    if (str == "x")
        return YafAxis::X;
    else if (str == "y")
        return YafAxis::Y;
    else if (str == "z")
        return YafAxis::Z;
    else
        throw YafParsingException("Invalid axis " + str + " used");
}

enum YafDrawMode
{
    Fill,
    Line,
    Point
};

inline YafDrawMode YafDrawModeFromString(const std::string& str)
{
    if (str == "fill")
        return YafDrawMode::Fill;
    else if (str == "line")
        return YafDrawMode::Line;
    else if (str == "point")
        return YafDrawMode::Point;
    else
        throw YafParsingException("Invalid draw mode " + str + " used");
}

enum YafShading
{
    Flat,
    Gouraud
};

inline YafShading YafShadingFromString(const std::string& str)
{
    if (str == "flat")
        return YafShading::Flat;
    else if (str == "gouraud")
        return YafShading::Gouraud;
    else
        throw YafParsingException("Invalid shading " + str + " used");
}

enum YafCullFace
{
    None,
    Back,
    Front,
    Both
};

inline YafCullFace YafCullFaceFromString(const std::string& str)
{
    if (str == "none")
        return YafCullFace::None;
    else if (str == "back")
        return YafCullFace::Back;
    else if (str == "front")
        return YafCullFace::Front;
    else if (str == "both")
        return YafCullFace::Both;
    else
        throw YafParsingException("Invalid cull face " + str + " used");
}

enum YafCullOrder
{
    CCW,
    CW
};

inline YafCullOrder YafCullOrderFromString(const std::string& str)
{
    if (str == "CCW")
        return YafCullOrder::CCW;
    else if (str == "CW")
        return YafCullOrder::CW;
    else
        throw YafParsingException("Invalid cull order " + str + " used");
}

inline bool BoolFromString(const std::string& str)
{
    if (str == "true")
        return true;
    else if (str == "false")
        return false;
    else
        throw YafParsingException("Invalid bool " + str + " used");
}

inline GLenum YafToOpenGL(YafDrawMode dm) // glPolygonMode
{
    switch (dm)
    {
    case YafDrawMode::Fill:
        return GL_FILL;
    case YafDrawMode::Line:
        return GL_LINE;
    case YafDrawMode::Point:
        return GL_POINT;
    default:
        return GL_INVALID_ENUM;
    }
}

inline GLenum YafToOpenGL(YafShading s) // glShadeModel
{
    switch (s)
    {
    case YafShading::Flat:
        return GL_FLAT;
    case YafShading::Gouraud:
        return GL_SMOOTH;
    default:
        return GL_INVALID_ENUM;
    }
}

inline GLenum YafToOpenGL(YafCullFace cf) // glCullFace
{
    switch (cf)
    {
    case YafCullFace::None:
        return GL_NONE;
    case YafCullFace::Back:
        return GL_BACK;
    case YafCullFace::Front:
        return GL_FRONT;
    case YafCullFace::Both:
        return GL_FRONT_AND_BACK;
    default:
        return GL_INVALID_ENUM;
    }
}

inline GLenum YafToOpenGL(YafCullOrder co) // glFrontFace
{
    switch (co)
    {
    case YafCullOrder::CCW:
        return GL_CCW;
    case YafCullOrder::CW:
        return GL_CW;
    default:
        return GL_INVALID_ENUM;
    }
}

#endif // YafEnums_h__
