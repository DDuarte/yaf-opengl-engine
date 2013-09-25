#include <tinyxml.h>
#include <cstdlib>

#include "YafScene.h"
#include "YafNode.h"
#include "TinyXMLYafHelpers.h"
#include "YafInterface.h"
#include "YafAppearance.h"

#include <CGFapplication.h>

YafScene* ParseYafFile(const std::string& file)
{
    auto scene = new YafScene;

    auto document = new TiXmlDocument(file);
    if (!document->LoadFile())
        throw YafParsingException("Could not load file '" + file + "'. Error: " + std::string(document->ErrorDesc()));

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

    for (auto cp = camerasPerspective.begin(); cp != camerasPerspective.end(); ++cp)
    {
        YafPerspectiveCamera* camera = new YafPerspectiveCamera(GetAttribute<std::string>(*cp, "id", "cameras perspective"));
        camera->Near     = GetAttribute<float>(*cp, "near", "cameras perspective");
        camera->Far      = GetAttribute<float>(*cp, "far", "cameras perspective");
        camera->Angle    = GetAttribute<float>(*cp, "angle", "cameras perspective");
        camera->Position = GetAttribute<YafXYZ>(*cp, "pos", "cameras perspective");
        camera->Target   = GetAttribute<YafXYZ>(*cp, "target", "cameras perspective");

        scene->AddCamera(camera);
    }

    for (auto op = camerasOrtho.begin(); op != camerasOrtho.end(); ++op)
    {
        YafOrthoCamera* camera = new YafOrthoCamera(GetAttribute<std::string>(*op, "id", "cameras ortho"));
        camera->Near   = GetAttribute<float>(*op, "near", "cameras ortho");
        camera->Far    = GetAttribute<float>(*op, "far", "cameras ortho");
        camera->Left   = GetAttribute<float>(*op, "left", "cameras ortho");
        camera->Right  = GetAttribute<float>(*op, "right", "cameras ortho");
        camera->Top    = GetAttribute<float>(*op, "top", "cameras ortho");
        camera->Bottom = GetAttribute<float>(*op, "bottom", "cameras ortho");

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

    int i = 0;
    for (auto lo = lightsOmni.begin(); lo != lightsOmni.end(); ++lo)
    {
        YafOmniLight* light = new YafOmniLight(GetAttribute<std::string>(*lo, "id", "lighting omni"), 
                                               i++, GetAttribute<YafXYZ>(*lo, "location", "lighting omni"));
        light->Enabled = GetAttribute<bool>(*lo, "enabled", "lighting omni");
        light->Ambient = GetAttribute<YafRGBA>(*lo, "ambient", "lighting omni");
        light->Diffuse = GetAttribute<YafRGBA>(*lo, "diffuse", "lighting omni");
        light->Specular = GetAttribute<YafRGBA>(*lo, "specular", "lighting omni");
        scene->AddLight(light);
    }

    for (auto ls = lightsSpot.begin(); ls != lightsSpot.end(); ++ls)
    {
        YafSpotLight* light = new YafSpotLight(GetAttribute<std::string>(*ls, "id", "lighting spot"), 
                                               i++, GetAttribute<YafXYZ>(*ls, "location", "lighting spot"), 
                                               GetAttribute<YafXYZ>(*ls, "direction", "lighting spot"));
        light->Enabled = GetAttribute<bool>(*ls, "enabled", "lighting spot");
        light->Ambient = GetAttribute<YafRGBA>(*ls, "ambient", "lighting spot");
        light->Diffuse = GetAttribute<YafRGBA>(*ls, "diffuse", "lighting spot");
        light->Specular = GetAttribute<YafRGBA>(*ls, "specular", "lighting spot");
        light->Angle = GetAttribute<float>(*ls, "angle", "lighting spot");
        light->Exponent = GetAttribute<float>(*ls, "exponent", "lighting spot");
        scene->AddLight(light);
    }

    // <textures>

    auto texturesElement = yafElement->FirstChildElement("textures");
    if (!texturesElement)
        throw YafParsingException("<textures> not found");

    auto textures = GetAllChildren(texturesElement, "texture");

    for (auto t = textures.begin(); t != textures.end(); ++t)
    {
        YafTexture* tex = new YafTexture(GetAttribute<std::string>(*t, "id", "textures texture"),
                                         GetAttribute<std::string>(*t, "file", "textures texture"));
        scene->AddTexture(tex);
    }

    // <appearances>

    auto appearancesElement = yafElement->FirstChildElement("appearances");
    if (!appearancesElement)
        throw YafParsingException("<appearances> not found");

    auto appearances = GetAllChildren(appearancesElement, "appearance");

    for (auto a = appearances.begin(); a != appearances.end(); ++a)
    {
        YafAppearance* app = new YafAppearance(GetAttribute<std::string>(*a, "id", "appearances appearance"));
        app->Emissive = GetAttribute<YafRGBA>(*a, "emissive", "appearances appearance");
        app->Ambient = GetAttribute<YafRGBA>(*a, "ambient", "appearances appearance");
        app->Diffuse = GetAttribute<YafRGBA>(*a, "diffuse", "appearances appearance");
        app->Specular = GetAttribute<YafRGBA>(*a, "specular", "appearances appearance");
        app->Shininess = GetAttribute<float>(*a, "shininess", "appearances appearance");

        std::string texRef = GetAttribute<std::string>(*a, "textureref", "appearances appearance", false);
        if (texRef.empty())
        {
            app->Texture = nullptr;
            app->TexLengthS = 0.0f;
            app->TexLengthT = 0.0f;
        }
        else
        {
            app->Texture = scene->GetTexture(texRef);
            app->TexLengthS = GetAttribute<float>(*a, "texlength_s", "appearances appearance");
            app->TexLengthT = GetAttribute<float>(*a, "texlength_t", "appearances appearance");
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

    for (auto n = nodes.begin(); n != nodes.end(); ++n)
    {
        YafNode* node = new YafNode(GetAttribute<std::string>(*n, "id", "graph node"));

        std::string appRef = GetAttribute<std::string>(*n, "appearanceref", "graph node", false);
        node->SetAppearance(appRef.empty() ? nullptr : scene->GetAppearance(appRef));

        auto childrenElement = (*n)->FirstChildElement("children");
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

        for (auto r = childrenRectangle.begin(); r != childrenRectangle.end(); ++r)
        {
            YafRectangle* rect = new YafRectangle;
            rect->Point1 = GetAttribute<YafXY>(*r, "xy1", "graph node children rectangle");
            rect->Point2 = GetAttribute<YafXY>(*r, "xy2", "graph node children rectangle");

            node->AddChild(rect);
        }

        for (auto t = childrenTriangle.begin(); t != childrenTriangle.end(); ++t)
        {
            YafTriangle* tri = new YafTriangle(GetAttribute<YafXYZ>(*t, "xyz1", "graph node children triangle"),
                                               GetAttribute<YafXYZ>(*t, "xyz2", "graph node children triangle"),
                                               GetAttribute<YafXYZ>(*t, "xyz3", "graph node children triangle"));

            node->AddChild(tri);
        }

        for (auto c = childrenCylinder.begin(); c != childrenCylinder.end(); ++c)
        {
            YafCylinder* cy = new YafCylinder;
            cy->Base = GetAttribute<float>(*c, "base", "graph node children cylinder");
            cy->Top = GetAttribute<float>(*c, "top", "graph node children cylinder");
            cy->Height = GetAttribute<float>(*c, "height", "graph node children cylinder");
            cy->Slices = GetAttribute<int>(*c, "slices", "graph node children cylinder");
            cy->Stacks = GetAttribute<int>(*c, "stacks", "graph node children cylinder");

            node->AddChild(cy);
        }

        for (auto s = childrenSphere.begin(); s != childrenSphere.end(); ++s)
        {
            YafSphere* sph = new YafSphere;
            sph->Radius = GetAttribute<float>(*s, "radius", "graph node children sphere");
            sph->Slices = GetAttribute<int>(*s, "slices", "graph node children sphere");
            sph->Stacks = GetAttribute<int>(*s, "stacks", "graph node children sphere");

            node->AddChild(sph);
        }

        for (auto t = childrenTorus.begin(); t != childrenTorus.end(); ++t)
        {
            YafTorus* tor = new YafTorus;
            tor->Inner = GetAttribute<float>(*t, "inner", "graph node children torus");
            tor->Outer = GetAttribute<float>(*t, "outer", "graph node children torus");
            tor->Slices = GetAttribute<int>(*t, "slices", "graph node children torus");
            tor->Loops = GetAttribute<int>(*t, "loops", "graph node children torus");

            node->AddChild(tor);
        }

        for (auto n = childrenNodeRef.begin(); n != childrenNodeRef.end(); ++n)
        {
            std::string nodeRef = GetAttribute<std::string>(*n, "id", "graph node children noderef");
            node->AddNodeRef(nodeRef);
        }

        scene->AddNode(node);
    }

    scene->DoPostProcessing();

    return scene;
}

int main(int argc, char* argv[])
{
    YafScene* scene = nullptr;

    try
    {
        scene = ParseYafFile("sintax_yaf2.xml");
    }
    catch (YafParsingException& ex)
    {
        std::cerr << "Exception while parsing caught: " << ex.what() << std::endl;
        std::cout << "Press ENTER key to continue." << std::endl;
        std::cin.get();
        return EXIT_FAILURE;
    }
    catch (GLexception& ex)
    {
        std::cerr << "Glexception while parsing caught: " << ex.what() << std::endl;
        std::cout << "Press ENTER key to continue." << std::endl;
        std::cin.get();
        return EXIT_FAILURE;
    }

    CGFapplication app;

    try
    {
        app.init(&argc, argv);

        app.setScene(scene);
        app.setInterface(new YafInterface());

        app.run();
    }
    catch (GLexception& ex)
    {
        std::cerr << "GLexception: " << ex.what() << std::endl;
        std::cout << "Press ENTER key to continue." << std::endl;
        std::cin.get();
        return EXIT_FAILURE;
    }
    catch (std::exception& ex)
    {
        std::cerr << "Exception: " << ex.what() << std::endl;
        std::cout << "Press ENTER key to continue." << std::endl;
        std::cin.get();
        return EXIT_FAILURE;
    }

    std::cout << "Press ENTER key to continue." << std::endl;
    std::cin.get();
    return EXIT_SUCCESS;
}
