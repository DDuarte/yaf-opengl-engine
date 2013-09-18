#include <tinyxml.h>
#include <cstdlib>

//#include <gl/GL.h>

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

enum class YafAxis
{
    X,
    Y,
    Z
};

enum class YafDrawMode
{
    Fill,
    Line,
    Point
};

enum class YafShading
{
    Flat,
    Gouraud
};

enum class YafCullFace
{
    None,
    Back,
    Front,
    Both
};

enum class YafCullOrder
{
    CCW,
    CW
};

// unsigned int /* GLenum */ YafToOpenGL(YafDrawMode dm) // glPolygonMode
// {
//     switch (dm)
//     {
//     case YafDrawMode::Fill:
//         return GL_FILL;
//     case YafDrawMode::Line:
//         return GL_LINE;
//     case YafDrawMode::Point:
//         return GL_POINT;
//     default:
//         return GL_INVALID_ENUM;
//     }
// }
// 
// unsigned int /* GLenum */ YafToOpenGL(YafShading s) // glShadeModel
// {
//     switch (s)
//     {
//     case YafShading::Flat:
//         return GL_FLAT;
//     case YafShading::Gouraud:
//         return GL_SMOOTH;
//     default:
//         return GL_INVALID_ENUM;
//     }
// }
// 
// unsigned int /* GLenum */ YafToOpenGL(YafCullFace cf) // glCullFace
// {
//     switch (cf)
//     {
//     case YafCullFace::None:
//         return GL_NONE;
//     case YafCullFace::Back:
//         return GL_BACK;
//     case YafCullFace::Front:
//         return GL_FRONT;
//     case YafCullFace::Both:
//         return GL_FRONT_AND_BACK;
//     default:
//         return GL_INVALID_ENUM;
//     }
// }
// 
// unsigned int /* GLenum */ YafToOpenGL(YafCullOrder co) // glFrontFace
// {
//     switch (co)
//     {
//     case YafCullOrder::CCW:
//         return GL_CCW;
//     case YafCullOrder::CW:
//         return GL_CW;
//     default:
//         return GL_INVALID_ENUM;
//     }
// }

class YafXY
{
public:
    float X;
    float Y;
};

class YafXYZ
{
public:
    float X;
    float Y;
    float Z;
};

class YafRGBA
{
public:
    float R;
    float G;
    float B;
    float A;
};

class YafCamera : public YafElement
{
public:
    float Near;
    float Far;
};

class YafOmniCamera : public YafCamera
{
public:
    float Angle;
    YafXYZ Position;
    YafXYZ Target;
};

class YafOrthoCamera : public YafCamera
{
    float Left;
    float Right;
    float Top;
    float Bottom;
};

class YafLight : public YafElement
{
    bool Enabled;
    YafXYZ Location;
    YafRGBA Ambient;
    YafRGBA Diffuse;
    YafRGBA Specular;
};

class YafOmniLight : public YafLight
{

};

class YafSpotLight : public YafLight
{
    float Angle;
    float Exponent;
    YafXYZ Direction;
};

class YafTexture : public YafElement
{
    std::string File;
};

class YafAppearance : public YafElement
{
    YafRGBA Emissive;
    YafRGBA Ambient;
    YafRGBA Diffuse;
    YafRGBA Specular;
    float Shininess;
    YafTexture Texture; // maybe ptr
    float TexLengthS;
    float TexLengthT;
};

// TODO: YafNode, YafChildren

class YafTransform
{

};

class YafTranslateTransform : public YafTransform
{
public:
    YafXYZ To;
};

class YafRotateTransform : public YafTransform
{
public:
    YafAxis Axis;
    float Angle;
};

class YafScaleTransform : public YafTransform
{
public:
    YafXYZ Factor;
};

class YafPrimitive : public /* ? */ YafElement
{

};

class YafRectangle : public YafPrimitive
{
public:
    YafXY Point1;
    YafXY Point2;
};

class YafTriangle : public YafPrimitive
{
public:
    YafXYZ Point1;
    YafXYZ Point2;
    YafXYZ Point3;
};

class YafCylinder : public YafPrimitive
{
public:
    float Base;
    float Top;
    float Height;
    int Slices;
    int Stacks;
};

class YafSphere : public YafPrimitive
{
public:
    float Radius;
    int Slices;
    int Stacks;
};

class YafTorus : public YafPrimitive
{
public:
    float Inner;
    float Outer;
    int Slices;
    int Loops;
};

class YafScene
{
public:
    // TODO: methods
private:
    // Globals
    YafDrawMode _drawMode;
    YafShading _shading;
    YafCullFace _cullFace;
    YafCullOrder _cullOrder;

    // TODO: more members
};

void t()
{
    auto document = new TiXmlDocument("sintax_yaf.xml");
    bool loaded = document->LoadFile();
    if (!loaded)
        throw YafParsingException("Could not load file 'sintax_yaf.xml'. Error: " + std::string(document->ErrorDesc()));

    // <yaf>

    auto yafElement = document->FirstChildElement("yaf");
    if (!yafElement)
        throw YafParsingException("<yaf> not found");

    // <globals>

    auto globalsElement = yafElement->FirstChildElement("globals");
    if (!globalsElement)
        throw YafParsingException("<globals> not found");

    std::string globalsBackgroundStr; // RBBA floats
    std::string globalsDrawMode, globalsShading, globalsCullFace, globalsCullOrder;
    if (globalsElement->QueryValueAttribute("background", &globalsBackgroundStr) != TIXML_SUCCESS)
        throw YafParsingException("<globals background> not found");
    if (globalsElement->QueryValueAttribute("drawmode", &globalsDrawMode) != TIXML_SUCCESS)
        throw YafParsingException("<globals drawmode> not found");
    if (globalsElement->QueryValueAttribute("shading", &globalsShading) != TIXML_SUCCESS)
        throw YafParsingException("<globals shading> not found");
    if (globalsElement->QueryValueAttribute("cullface", &globalsCullFace) != TIXML_SUCCESS)
        throw YafParsingException("<globals cullface> not found");
    if (globalsElement->QueryValueAttribute("cullorder", &globalsCullOrder) != TIXML_SUCCESS)
        throw YafParsingException("<globals cullorder> not found");

    // <cameras>

    auto camerasElement = yafElement->FirstChildElement("cameras");
    if (!camerasElement)
        throw YafParsingException("<cameras> not found");

    std::string camerasInitial;
    if (camerasElement->QueryValueAttribute("initial", &camerasInitial) != TIXML_SUCCESS)
        throw YafParsingException("<cameras initial> not found");

    // TODO: perpendicular/ortho

    // <lighting>

    auto lightingElement = yafElement->FirstChildElement("lighting");
    if (!lightingElement)
        throw YafParsingException("<lighting> not found");

    std::string lightingDoubleSidedStr, lightingLocalStr, lightingEnabledStr; // bools
    std::string lightingAmbientStr; // RGBA floats
    if (lightingElement->QueryValueAttribute("doublesided", &lightingDoubleSidedStr) != TIXML_SUCCESS)
        throw YafParsingException("<lighting doublesided> not found");
    if (lightingElement->QueryValueAttribute("local", &lightingLocalStr) != TIXML_SUCCESS)
        throw YafParsingException("<lighting local> not found");
    if (lightingElement->QueryValueAttribute("enabled", &lightingEnabledStr) != TIXML_SUCCESS)
        throw YafParsingException("<lighting enabled> not found");
    if (lightingElement->QueryValueAttribute("ambient", &lightingAmbientStr) != TIXML_SUCCESS)
        throw YafParsingException("<lighting ambient> not found");

    // TODO: omni/spot

    // <textures>

    auto texturesElement = yafElement->FirstChildElement("textures");
    if (!texturesElement)
        throw YafParsingException("<textures> not found");

    // TODO: texture

    // <appearances>

    auto appearancesElement = yafElement->FirstChildElement("appearances");
    if (!appearancesElement)
        throw YafParsingException("<appearances> not found");

    // TODO: appearance

    // <graph>

    auto graphElement = yafElement->FirstChildElement("graph");
    if (!graphElement)
        throw YafParsingException("<graph> not found");

    std::string graphRootId;
    if (graphElement->QueryValueAttribute("rootid", &graphRootId) != TIXML_SUCCESS)
        throw YafParsingException("<graph rootid> not found");

    // TODO: node
}

int main(int argc, char* argv[])
{
    try
    {
        t();
    }
    catch (YafParsingException& ex)
    {
    	std::cout << "Exception caught: " << ex.what() << std::endl;
    }

    system("PAUSE");
    return EXIT_SUCCESS;
}
