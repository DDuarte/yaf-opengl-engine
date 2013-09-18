#include <tinyxml.h>
#include <cstdlib>
#include <map>
#include <vector>

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

template<typename T>
std::vector<T> ConcatenateVectors(std::vector<T>& v1, std::vector<T>& v2)
{
    std::vector<T> result = v1;
    result.insert(result.end(), v2.begin(), v2.end());
    return result;
}

enum class YafAxis
{
    X,
    Y,
    Z
};

YafAxis YafAxisFromString(const std::string& str)
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

enum class YafDrawMode
{
    Fill,
    Line,
    Point
};

YafDrawMode YafDrawModeFromString(const std::string& str)
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

enum class YafShading
{
    Flat,
    Gouraud
};

YafShading YafShadingFromString(const std::string& str)
{
    if (str == "flat")
        return YafShading::Flat;
    else if (str == "gouraud")
        return YafShading::Gouraud;
    else
        throw YafParsingException("Invalid shading " + str + " used");
}

enum class YafCullFace
{
    None,
    Back,
    Front,
    Both
};

YafCullFace YafCullFaceFromString(const std::string& str)
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

enum class YafCullOrder
{
    CCW,
    CW
};

YafCullOrder YafCullOrderFromString(const std::string& str)
{
    if (str == "CCW")
        return YafCullOrder::CCW;
    else if (str == "CW")
        return YafCullOrder::CW;
    else
        throw YafParsingException("Invalid cull order " + str + " used");
}

bool BoolFromString(const std::string& str)
{
    if (str == "true")
        return true;
    else if (str == "false")
        return false;
    else
        throw YafParsingException("Invalid bool " + str + " used");
}

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

std::vector<float> ParseFloats(std::string s , int n) 
{
    std::vector<float> floats;

    char* str = const_cast<char*>(s.c_str());
    char* pch;
    char* context = nullptr;

    pch = strtok_s(str, " ", &context);
    while (pch)
    {
        floats.push_back(static_cast<float>(atof(pch)));
        pch = strtok_s(nullptr, " ", &context);
    }

    if (floats.size() != n)
        throw YafParsingException("Incorrect number of arguments: " + s);

    return floats;
}

class YafXY
{
public:
    YafXY() : X(0.0f), Y(0.0f) { }

    YafXY(const std::string& s)
    {
        std::vector<float> XY = ParseFloats(s, 2);

        X = XY[0];
        Y = XY[1];
    }

    float X;
    float Y;
};

class YafXYZ
{
public:
    YafXYZ() : X(0.0f), Y(0.0f), Z(0.0f) { }

    YafXYZ(const std::string& s)
    {
        std::vector<float> XYZ = ParseFloats(s, 3);

        X = XYZ[0];
        Y = XYZ[1];
        Z = XYZ[2];
    }

    float X;
    float Y;
    float Z;
};

class YafRGBA
{
public:
    YafRGBA() : R(0.0f), G(0.0f), B(0.0f), A(0.0f) { }

    YafRGBA(const std::string& s)
    {
        std::vector<float> RGBA = ParseFloats(s, 4);

        R = RGBA[0];
        G = RGBA[1];
        B = RGBA[2];
        A = RGBA[3];
    }

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

class YafPerspectiveCamera : public YafCamera
{
public:
    float Angle;
    YafXYZ Position;
    YafXYZ Target;
};

class YafOrthoCamera : public YafCamera
{
public:
    float Left;
    float Right;
    float Top;
    float Bottom;
};

class YafLight : public YafElement
{
public:
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
public:
    float Angle;
    float Exponent;
    YafXYZ Direction;
};

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

class YafChild
{

};

class YafPrimitive : public YafChild
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

class YafNode;


class YafScene
{
public:
    YafScene() { };

    void SetGlobals(YafRGBA bg, YafDrawMode dm, YafShading s, YafCullFace cf, YafCullOrder co)
    {
        _backgroundColor = bg;
        _drawMode = dm;
        _shading = s;
        _cullFace = cf;
        _cullOrder = co;
    }

    void SetLightOptions(bool doubleSided, bool local, bool enabled, YafRGBA ambient)
    {
        _lightDoubleSided = doubleSided;
        _lightLocal = local;
        _lightEnabled = enabled;
        _lightAmbient = ambient;
    }

    void SetInitialCamera(const std::string& id) { _initialCamera = _cameras[id]; }
    void SetRootNode(const std::string& id) { _rootNode = _nodes[id]; }

    void AddCamera(YafCamera* camera) { _cameras[camera->Id] = camera; }
    void AddLight(YafLight* light) { _lights[light->Id] = light; }
    void AddTexture(YafTexture* texture) { _textures[texture->Id] = texture; }
    void AddAppearance(YafAppearance* appearance) { _appearances[appearance->Id] = appearance; }
    void AddNode(YafNode* node) { _nodes[node->Id] = node; }

    YafTexture* GetTexture(const std::string& id) const { return _textures.find(id)->second; }
    YafAppearance* GetAppearance(const std::string& id) const { return _appearances.find(id)->second; }
    YafLight* GetLight(const std::string& id) const { return _lights.find(id)->second; }
    YafNode* GetNode(const std::string& id) const { return _nodes.find(id)->second; }

    std::map<std::string, YafNode*>& GetNodes() { return _nodes; }

private:
    // Globals
    YafRGBA _backgroundColor;
    YafDrawMode _drawMode;
    YafShading _shading;
    YafCullFace _cullFace;
    YafCullOrder _cullOrder;

    // Cameras
    YafCamera* _initialCamera;
    std::map<std::string, YafCamera*> _cameras;

    // Lighting
    bool _lightDoubleSided;
    bool _lightLocal;
    bool _lightEnabled;
    YafRGBA _lightAmbient;
    std::map<std::string, YafLight*> _lights;

    // Textures
    std::map<std::string, YafTexture*> _textures;

    // Appearances
    std::map<std::string, YafAppearance*> _appearances;

    // Graph
    YafNode* _rootNode;
    std::map<std::string, YafNode*> _nodes;
};

class YafNode : public YafChild, public YafElement
{
public:
    void AddTransform(YafTransform* t) { _transforms.push_back(t); }
    void AddChild(YafChild* c) { _children.push_back(c); }
    void SetAppearance(YafAppearance* a) { _appearance = a; }

    void AddNodeRef(const std::string& id) { _refNodes.push_back(id); }

    void MoveRefNodesToChildren(YafScene* scene)
    {
        for (auto s : _refNodes)
            AddChild(scene->GetNode(s));

        _refNodes.clear();
    }

private:
    std::vector<YafTransform*> _transforms;
    YafAppearance* _appearance; // can be null

    std::vector<YafChild*> _children;

    std::vector<std::string> _refNodes;
};



std::vector<TiXmlElement*> GetAllChildren(TiXmlElement* root, const std::string& name)
{
    std::vector<TiXmlElement*> children;

    if (root)
    {
        if (TiXmlElement* first = root->FirstChildElement(name))
        {
            children.push_back(first);

            TiXmlElement* next = first->NextSiblingElement(name);

            while (next)
            {
                children.push_back(next);
                next = next->NextSiblingElement(name);
            } 
        }
    }

    return children;
}

template<typename T>
T GetAttribute(const TiXmlElement* element, const std::string& name, const std::string& prefix, bool required = true)
{
    T res;
    if (element->QueryValueAttribute(name, &res) != TIXML_SUCCESS)
        if (required)
            throw YafParsingException("<" + prefix + " " + name + "> not found");
        else
            return T();
    return res;
}

template<>
bool GetAttribute(const TiXmlElement* element, const std::string& name, const std::string& prefix, bool required /* = true */)
{
    return BoolFromString(GetAttribute<std::string>(element, name, prefix, required));
}

template<>
YafRGBA GetAttribute(const TiXmlElement* element, const std::string& name, const std::string& prefix, bool required /* = true */)
{
    return YafRGBA(GetAttribute<std::string>(element, name, prefix, required));
}

template<>
YafXY GetAttribute(const TiXmlElement* element, const std::string& name, const std::string& prefix, bool required /* = true */)
{
    return YafXY(GetAttribute<std::string>(element, name, prefix, required));
}

template<>
YafXYZ GetAttribute(const TiXmlElement* element, const std::string& name, const std::string& prefix, bool required /* = true */)
{
    return YafXYZ(GetAttribute<std::string>(element, name, prefix, required));
}

void t()
{
    auto scene = new YafScene;

    auto document = new TiXmlDocument("sintax_yaf2.xml");
    if (!document->LoadFile())
        throw YafParsingException("Could not load file 'sintax_yaf.xml'. Error: " + std::string(document->ErrorDesc()));

    // <yaf>

    auto yafElement = document->FirstChildElement("yaf");
    if (!yafElement)
        throw YafParsingException("<yaf> not found");

    // <globals>

    auto globalsElement = yafElement->FirstChildElement("globals");
    if (!globalsElement)
        throw YafParsingException("<globals> not found");

    scene->SetGlobals(GetAttribute<YafRGBA>(globalsElement, "background", "globals"),
                      YafDrawModeFromString(GetAttribute<std::string>(globalsElement, "drawmode", "globals")),
                      YafShadingFromString(GetAttribute<std::string>(globalsElement, "shading", "globals")),
                      YafCullFaceFromString(GetAttribute<std::string>(globalsElement, "cullface", "globals")),
                      YafCullOrderFromString(GetAttribute<std::string>(globalsElement, "cullorder", "globals")));

    // <cameras>

    auto camerasElement = yafElement->FirstChildElement("cameras");
    if (!camerasElement)
        throw YafParsingException("<cameras> not found");

    auto camerasPerspective = GetAllChildren(camerasElement, "perspective");
    auto camerasOrtho = GetAllChildren(camerasElement, "ortho");

    for (auto cp : camerasPerspective)
    {
        YafPerspectiveCamera* camera = new YafPerspectiveCamera;
        camera->Id       = GetAttribute<std::string>(cp, "id", "cameras perspective");
        camera->Near     = GetAttribute<float>(cp, "near", "cameras perspective");
        camera->Far      = GetAttribute<float>(cp, "far", "cameras perspective");
        camera->Angle    = GetAttribute<float>(cp, "angle", "cameras perspective");
        camera->Position = GetAttribute<YafXYZ>(cp, "pos", "cameras perspective");
        camera->Target   = GetAttribute<YafXYZ>(cp, "target", "cameras perspective");

        scene->AddCamera(camera);
    }

    for (auto op : camerasOrtho)
    {
        YafOrthoCamera* camera = new YafOrthoCamera;
        camera->Id     = GetAttribute<std::string>(op, "id", "cameras ortho");
        camera->Near   = GetAttribute<float>(op, "near", "cameras ortho");
        camera->Far    = GetAttribute<float>(op, "far", "cameras ortho");
        camera->Left   = GetAttribute<float>(op, "left", "cameras ortho");
        camera->Right  = GetAttribute<float>(op, "right", "cameras ortho");
        camera->Top    = GetAttribute<float>(op, "top", "cameras ortho");
        camera->Bottom = GetAttribute<float>(op, "bottom", "cameras ortho");

        scene->AddCamera(camera);
    }

    scene->SetInitialCamera(GetAttribute<std::string>(camerasElement, "initial", "cameras"));

    // <lighting>

    auto lightingElement = yafElement->FirstChildElement("lighting");
    if (!lightingElement)
        throw YafParsingException("<lighting> not found");

    scene->SetLightOptions(GetAttribute<bool>(lightingElement, "doublesided", "lighting"),
                           GetAttribute<bool>(lightingElement, "local", "lighting"),
                           GetAttribute<bool>(lightingElement, "enabled", "lighting"),
                           GetAttribute<YafRGBA>(lightingElement, "ambient", "lighting"));

    auto lightsOmni = GetAllChildren(lightingElement, "omni");
    auto lightsSpot = GetAllChildren(lightingElement, "spot");

    for (auto lo : lightsOmni)
    {
        YafOmniLight* light = new YafOmniLight;
        light->Id = GetAttribute<std::string>(lo, "id", "lighting omni");
        light->Enabled = GetAttribute<bool>(lo, "enabled", "lighting omni");
        light->Location = GetAttribute<YafXYZ>(lo, "location", "lighting omni");
        light->Ambient = GetAttribute<YafRGBA>(lo, "ambient", "lighting omni");
        light->Diffuse = GetAttribute<YafRGBA>(lo, "diffuse", "lighting omni");
        light->Specular = GetAttribute<YafRGBA>(lo, "specular", "lighting omni");

        scene->AddLight(light);
    }

    for (auto lo : lightsSpot)
    {
        YafSpotLight* light = new YafSpotLight;
        light->Id = GetAttribute<std::string>(lo, "id", "lighting spot");
        light->Enabled = GetAttribute<bool>(lo, "enabled", "lighting spot");
        light->Location = GetAttribute<YafXYZ>(lo, "location", "lighting spot");
        light->Ambient = GetAttribute<YafRGBA>(lo, "ambient", "lighting spot");
        light->Diffuse = GetAttribute<YafRGBA>(lo, "diffuse", "lighting spot");
        light->Specular = GetAttribute<YafRGBA>(lo, "specular", "lighting spot");
        light->Angle = GetAttribute<float>(lo, "angle", "lighting spot");
        light->Exponent = GetAttribute<float>(lo, "exponent", "lighting spot");
        light->Direction = GetAttribute<YafXYZ>(lo, "direction", "lighting spot");

        scene->AddLight(light);
    }

    // <textures>

    auto texturesElement = yafElement->FirstChildElement("textures");
    if (!texturesElement)
        throw YafParsingException("<textures> not found");

    auto textures = GetAllChildren(texturesElement, "texture");

    for (auto t : textures)
    {
        YafTexture* tex = new YafTexture;
        tex->Id = GetAttribute<std::string>(t, "id", "textures texture");
        tex->File = GetAttribute<std::string>(t, "file", "textures texture");

        scene->AddTexture(tex);
    }

    // <appearances>

    auto appearancesElement = yafElement->FirstChildElement("appearances");
    if (!appearancesElement)
        throw YafParsingException("<appearances> not found");

    auto appearances = GetAllChildren(appearancesElement, "appearance");

    for (auto a : appearances)
    {
        YafAppearance* app = new YafAppearance;
        app->Id = GetAttribute<std::string>(a, "id", "appearances appearance");
        app->Emissive = GetAttribute<YafRGBA>(a, "emissive", "appearances appearance");
        app->Ambient = GetAttribute<YafRGBA>(a, "ambient", "appearances appearance");
        app->Diffuse = GetAttribute<YafRGBA>(a, "diffuse", "appearances appearance");
        app->Specular = GetAttribute<YafRGBA>(a, "specular", "appearances appearance");
        app->Shininess = GetAttribute<float>(a, "shininess", "appearances appearance");

        std::string texRef = GetAttribute<std::string>(a, "textureref", "appearances appearance", false);
        if (texRef.empty())
        {
            app->Texture = nullptr;
            app->TexLengthS = 0.0f;
            app->TexLengthT = 0.0f;
        }
        else
        {
            app->Texture = scene->GetTexture(texRef);
            app->TexLengthS = GetAttribute<float>(a, "texlength_s", "appearances appearance");
            app->TexLengthT = GetAttribute<float>(a, "texlength_t", "appearances appearance");
        }

    }

    // <graph>

    auto graphElement = yafElement->FirstChildElement("graph");
    if (!graphElement)
        throw YafParsingException("<graph> not found");

    std::string graphRootId = GetAttribute<std::string>(graphElement, "rootid", "graph");

    auto nodes = GetAllChildren(graphElement, "node");

    if (nodes.empty())
        throw YafParsingException("<graph node> needs at least one node");

    for (auto n : nodes)
    {
        YafNode* node = new YafNode;
        node->Id = GetAttribute<std::string>(n, "id", "graph node");
        
        std::string appRef = GetAttribute<std::string>(n, "appearanceref", "graph node", false);
        node->SetAppearance(appRef.empty() ? nullptr : scene->GetAppearance(appRef));

        auto childrenElement = n->FirstChildElement("children");
        if (!childrenElement)
            throw YafParsingException("<graph node children> not found");

        auto childrenRectangle = GetAllChildren(childrenElement, "rectangle");
        auto childrenTriangle = GetAllChildren(childrenElement, "triangle");
        auto childrenCylinder = GetAllChildren(childrenElement, "cylinder");
        auto childrenSphere = GetAllChildren(childrenElement, "sphere");
        auto childrenTorus = GetAllChildren(childrenElement, "torus");
        auto childrenNodeRef = GetAllChildren(childrenElement, "noderef");

        if (childrenRectangle.empty() && childrenTriangle.empty() &&
            childrenCylinder.empty() && childrenSphere.empty() &&
            childrenTorus.empty() && childrenNodeRef.empty())
            throw YafParsingException("<graph node children> needs at least one primitive or node");

        for (auto r : childrenRectangle)
        {
            YafRectangle* rect = new YafRectangle;
            rect->Point1 = GetAttribute<YafXY>(r, "xy1", "graph node children rectangle");
            rect->Point2 = GetAttribute<YafXY>(r, "xy2", "graph node children rectangle");

            node->AddChild(rect);
        }

        for (auto t : childrenTriangle)
        {
            YafTriangle* tri = new YafTriangle;
            tri->Point1 = GetAttribute<YafXYZ>(t, "xyz1", "graph node children triangle");
            tri->Point2 = GetAttribute<YafXYZ>(t, "xyz2", "graph node children triangle");
            tri->Point3 = GetAttribute<YafXYZ>(t, "xyz3", "graph node children triangle");

            node->AddChild(tri);
        }

        for (auto c : childrenCylinder)
        {
            YafCylinder* cy = new YafCylinder;
            cy->Base = GetAttribute<float>(c, "base", "graph node children cylinder");
            cy->Top = GetAttribute<float>(c, "top", "graph node children cylinder");
            cy->Height = GetAttribute<float>(c, "height", "graph node children cylinder");
            cy->Slices = GetAttribute<int>(c, "slices", "graph node children cylinder");
            cy->Stacks = GetAttribute<int>(c, "stacks", "graph node children cylinder");

            node->AddChild(cy);
        }

        for (auto s : childrenSphere)
        {
            YafSphere* sph = new YafSphere;
            sph->Radius = GetAttribute<float>(s, "radius", "graph node children sphere");
            sph->Slices = GetAttribute<int>(s, "slices", "graph node children sphere");
            sph->Stacks = GetAttribute<int>(s, "stacks", "graph node children sphere");

            node->AddChild(sph);
        }

        for (auto t : childrenTorus)
        {
            YafTorus* tor = new YafTorus;
            tor->Inner = GetAttribute<float>(t, "inner", "graph node children torus");
            tor->Outer = GetAttribute<float>(t, "outer", "graph node children torus");
            tor->Slices = GetAttribute<int>(t, "slices", "graph node children torus");
            tor->Loops = GetAttribute<int>(t, "loops", "graph node children torus");

            node->AddChild(tor);
        }

        for (auto n : childrenNodeRef)
        {
            std::string nodeRef = GetAttribute<std::string>(n, "id", "graph node children noderef");
            node->AddNodeRef(nodeRef);
        }

        scene->AddNode(node);
    }

    for (auto n : scene->GetNodes())
        n.second->MoveRefNodesToChildren(scene);
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
