#include <tinyxml.h>
#include <cstdlib>
#include <string>
#include <CGFapplication.h>
#include "Main.h"
#include "YafScene.h"
#include "YafNode.h"
#include "TinyXMLYafHelpers.h"
#include "YafInterface.h"
#include "YafAppearance.h"
#include "Game.h"

YafScene* ParseYafFile(const std::string& file)
{
    auto scene = new YafScene;
    scene->SetYafName(file);

    auto document = new TiXmlDocument(file);
    if (!document->LoadFile())
        throw YafParsingException("Could not load file '" + file + "'. Error: " + std::string(document->ErrorDesc()));

    // <yaf>

    auto yafElement = GetChildren(document, "yaf", "");

    // <globals>

    auto globalsElement = GetChildren(yafElement, "globals", "yaf");

    auto background = GetAttribute<YafRGBA>(globalsElement, "background", "globals");
    auto drawMode = YafDrawModeFromString(GetAttribute<std::string>(globalsElement, "drawmode", "globals"));
    auto shading = YafShadingFromString(GetAttribute<std::string>(globalsElement, "shading", "globals"));
    auto cullFace = YafCullFaceFromString(GetAttribute<std::string>(globalsElement, "cullface", "globals"));
    auto cullOrder = YafCullOrderFromString(GetAttribute<std::string>(globalsElement, "cullorder", "globals"));

    scene->SetGlobals(background, drawMode, shading, cullFace, cullOrder);

    // <cameras>

    auto camerasElement = GetChildren(yafElement, "cameras", "yaf");

    auto camerasPerspective = GetAllChildren(camerasElement, "perspective");
    auto camerasOrtho = GetAllChildren(camerasElement, "ortho");
    auto camerasFree = GetAllChildren(camerasElement, "free");

    if (camerasPerspective.empty() && camerasOrtho.empty() && camerasFree.empty())
        throw YafParsingException("<yaf cameras> needs at least one camera");

    for (auto cp = camerasPerspective.begin(); cp != camerasPerspective.end(); ++cp)
    {
        auto id = GetAttribute<std::string>(*cp, "id", "cameras perspective");

        auto position = GetAttribute<YafXYZ>(*cp, "pos", "cameras perspective");

        auto camera = new YafPerspectiveCamera(id, position);
        camera->Near     = GetAttribute<float>(*cp, "near", "cameras perspective");
        camera->Far      = GetAttribute<float>(*cp, "far", "cameras perspective");
        camera->Angle    = GetAttribute<float>(*cp, "angle", "cameras perspective");
        camera->Target   = GetAttribute<YafXYZ>(*cp, "target", "cameras perspective");
        scene->AddCamera(camera);
    }

    for (auto op = camerasOrtho.begin(); op != camerasOrtho.end(); ++op)
    {
        auto id = GetAttribute<std::string>(*op, "id", "cameras ortho");
        auto camera = new YafOrthoCamera(id);
        camera->Near   = GetAttribute<float>(*op, "near", "cameras ortho");
        camera->Far    = GetAttribute<float>(*op, "far", "cameras ortho");
        camera->Left   = GetAttribute<float>(*op, "left", "cameras ortho");
        camera->Right  = GetAttribute<float>(*op, "right", "cameras ortho");
        camera->Top    = GetAttribute<float>(*op, "top", "cameras ortho");
        camera->Bottom = GetAttribute<float>(*op, "bottom", "cameras ortho");

        scene->AddCamera(camera);
    }

    for (auto cf = camerasFree.begin(); cf != camerasFree.end(); ++cf)
    {
        auto id = GetAttribute<std::string>(*cf, "id", "cameras free");

        auto position = GetAttribute<YafXYZ>(*cf, "pos", "cameras free");

        auto camera = new YafFreePersCamera(id, position);
        camera->Near = GetAttribute<float>(*cf, "near", "cameras free");
        camera->Far = GetAttribute<float>(*cf, "far", "cameras free");
        camera->Angle = GetAttribute<float>(*cf, "angle", "cameras free");
        camera->Target = GetAttribute<YafXYZ>(*cf, "target", "cameras free");
        scene->AddCamera(camera);
    }

    scene->SetInitialCamera(GetAttribute<std::string>(camerasElement, "initial", "cameras"));

    // <lighting>

    auto lightingElement = GetChildren(yafElement, "lighting", "yaf");

    auto doubleSided = GetAttribute<bool>(lightingElement, "doublesided", "lighting");
    auto local = GetAttribute<bool>(lightingElement, "local", "lighting");
    auto enabled = GetAttribute<bool>(lightingElement, "enabled", "lighting");
    auto ambient = GetAttribute<YafRGBA>(lightingElement, "ambient", "lighting");

    scene->SetLightOptions(doubleSided, local, enabled, ambient);

    auto lightsOmni = GetAllChildren(lightingElement, "omni");
    auto lightsSpot = GetAllChildren(lightingElement, "spot");

    int i = 0;
    for (auto lo = lightsOmni.begin(); lo != lightsOmni.end(); ++lo)
    {
        auto id = GetAttribute<std::string>(*lo, "id", "lighting omni");
        auto location = GetAttribute<YafXYZ>(*lo, "location", "lighting omni");
        auto light = new YafOmniLight(id, i++, YafXYZW(location, 1.0f));
        light->Enabled = GetAttribute<bool>(*lo, "enabled", "lighting omni");
        light->Ambient = GetAttribute<YafRGBA>(*lo, "ambient", "lighting omni");
        light->Diffuse = GetAttribute<YafRGBA>(*lo, "diffuse", "lighting omni");
        light->Specular = GetAttribute<YafRGBA>(*lo, "specular", "lighting omni");
        scene->AddLight(light);
    }

    for (auto ls = lightsSpot.begin(); ls != lightsSpot.end(); ++ls)
    {
        auto id = GetAttribute<std::string>(*ls, "id", "lighting spot");
        auto location = GetAttribute<YafXYZ>(*ls, "location", "lighting spot");
        auto direction = GetAttribute<YafXYZ>(*ls, "direction", "lighting spot");
        YafSpotLight* light = new YafSpotLight(id, i++, YafXYZW(location, 1.0f), direction);
        light->Enabled = GetAttribute<bool>(*ls, "enabled", "lighting spot");
        light->Ambient = GetAttribute<YafRGBA>(*ls, "ambient", "lighting spot");
        light->Diffuse = GetAttribute<YafRGBA>(*ls, "diffuse", "lighting spot");
        light->Specular = GetAttribute<YafRGBA>(*ls, "specular", "lighting spot");
        light->Angle = GetAttribute<float>(*ls, "angle", "lighting spot");
        light->Exponent = GetAttribute<float>(*ls, "exponent", "lighting spot");
        scene->AddLight(light);
    }

    // <textures>

    auto texturesElement = GetChildren(yafElement, "textures", "yaf");

    auto textures = GetAllChildren(texturesElement, "texture");

    for (auto t = textures.begin(); t != textures.end(); ++t)
    {
        auto id = GetAttribute<std::string>(*t, "id", "textures texture");
        auto file = GetAttribute<std::string>(*t, "file", "textures texture");
        auto tex = new YafTexture(id, file);
        scene->AddTexture(tex);
    }

    // <appearances>

    auto appearancesElement = GetChildren(yafElement, "appearances", "yaf");

    auto appearances = GetAllChildren(appearancesElement, "appearance");

    for (auto a = appearances.begin(); a != appearances.end(); ++a)
    {
        auto app = new YafAppearance(GetAttribute<std::string>(*a, "id", "appearances appearance"));
        app->Emissive = GetAttribute<YafRGBA>(*a, "emissive", "appearances appearance");
        app->Ambient = GetAttribute<YafRGBA>(*a, "ambient", "appearances appearance");
        app->Diffuse = GetAttribute<YafRGBA>(*a, "diffuse", "appearances appearance");
        app->Specular = GetAttribute<YafRGBA>(*a, "specular", "appearances appearance");
        app->Shininess = GetAttribute<float>(*a, "shininess", "appearances appearance");

        auto texRef = GetAttribute<std::string>(*a, "textureref", "appearances appearance", false);
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

            if (app->TexLengthS == 0 || app->TexLengthT == 0)
                throw YafParsingException("appearances appearance texlength_s and texlength_t can't be 0."); // would mean division by 0 later
        }

        scene->AddAppearance(app);
    }

    // <graph>

    auto graphElement = GetChildren(yafElement, "graph", "yaf");

    auto graphRootId = GetAttribute<std::string>(graphElement, "rootid", "graph");

    auto nodes = GetAllChildren(graphElement, "node");

    if (nodes.empty())
        throw YafParsingException("<yaf graph node> needs at least one node");

    for (auto n = nodes.begin(); n != nodes.end(); ++n)
    {
        auto node = new YafNode(GetAttribute<std::string>(*n, "id", "graph node"));

        auto pickableStr = GetAttribute<std::string>(*n, "pickable", "graph node", false);
        node->Pickable = !pickableStr.empty() && BoolFromString(pickableStr);

        node->Position = GetAttribute<YafXYZ>(*n, "position", "graph node");
        node->Pitch = GetAttribute<float>(*n, "pitch", "graph node");
        node->Yaw = GetAttribute<float>(*n, "yaw", "graph node");
        node->Roll = GetAttribute<float>(*n, "roll", "graph node");
        node->Scale = GetAttribute<YafXYZ>(*n, "scale", "graph node");

        if (auto appRefElement = GetChildren(*n, "appearanceref", "graph node", false))
        {
            auto appRef = GetAttribute<std::string>(appRefElement, "id", "graph node appearanceref");
            node->SetAppearance(scene->GetAppearance(appRef));
        }
        else
            node->SetAppearance(nullptr);

        if (auto appSelectedRefElement = GetChildren(*n, "appearanceselectedref", "graph node", false))
        {
            auto appSelectedRef = GetAttribute<std::string>(appSelectedRefElement, "id", "graph node appearanceselectedref");
            node->SetAppearanceSelected(scene->GetAppearance(appSelectedRef));
        }
        else
            node->SetAppearanceSelected(nullptr);

        if (auto animationElement = GetChildren(*n, "animationref", "graph node", false))
        {
            auto anim = GetAttribute<std::string>(animationElement, "id", "graph node animationref");
            node->SetAnimation(scene->GetAnimation(anim));
        }
        else
            node->SetAnimation(nullptr);

        auto childrenElement = (*n)->FirstChildElement("children");
        if (!childrenElement)
            throw YafParsingException("<graph node children> not found");

        auto childrenRectangle = GetAllChildren(childrenElement, "rectangle");
        auto childrenTriangle = GetAllChildren(childrenElement, "triangle");
        auto childrenCylinder = GetAllChildren(childrenElement, "cylinder");
        auto childrenSphere = GetAllChildren(childrenElement, "sphere");
        auto childrenTorus = GetAllChildren(childrenElement, "torus");
        auto childrenNodeRef = GetAllChildren(childrenElement, "noderef");
        auto childrenPlane = GetAllChildren(childrenElement, "plane");
        auto childrenPatch = GetAllChildren(childrenElement, "patch");
        auto childrenWaterline = GetAllChildren(childrenElement, "waterline");

        if (childrenRectangle.empty() && childrenTriangle.empty() &&
            childrenCylinder.empty() && childrenSphere.empty() &&
            childrenTorus.empty() && childrenNodeRef.empty() &&
            childrenPlane.empty() && childrenPatch.empty() &&
            childrenWaterline.empty())
            throw YafParsingException("<graph node children> (" + node->Id + ") needs at least one primitive or node");

        for (auto r = childrenRectangle.begin(); r != childrenRectangle.end(); ++r)
        {
            auto rect = new YafRectangle;
            rect->Point1 = GetAttribute<YafXY>(*r, "xy1", "graph node children rectangle");
            rect->Point2 = GetAttribute<YafXY>(*r, "xy2", "graph node children rectangle");
            node->AddChild(rect);
        }

        for (auto t = childrenTriangle.begin(); t != childrenTriangle.end(); ++t)
        {
            auto p1 = GetAttribute<YafXYZ>(*t, "xyz1", "graph node children triangle");
            auto p2 = GetAttribute<YafXYZ>(*t, "xyz2", "graph node children triangle");
            auto p3 = GetAttribute<YafXYZ>(*t, "xyz3", "graph node children triangle");
            auto tri = new YafTriangle(p1, p2, p3);
            node->AddChild(tri);
        }

        for (auto c = childrenCylinder.begin(); c != childrenCylinder.end(); ++c)
        {
            auto cy = new YafCylinder;
            cy->Base = GetAttribute<float>(*c, "base", "graph node children cylinder");
            cy->Top = GetAttribute<float>(*c, "top", "graph node children cylinder");
            cy->Height = GetAttribute<float>(*c, "height", "graph node children cylinder");
            cy->Slices = GetAttribute<int>(*c, "slices", "graph node children cylinder");
            cy->Stacks = GetAttribute<int>(*c, "stacks", "graph node children cylinder");
            node->AddChild(cy);
        }

        for (auto s = childrenSphere.begin(); s != childrenSphere.end(); ++s)
        {
            auto sph = new YafSphere;
            sph->Radius = GetAttribute<float>(*s, "radius", "graph node children sphere");
            sph->Slices = GetAttribute<int>(*s, "slices", "graph node children sphere");
            sph->Stacks = GetAttribute<int>(*s, "stacks", "graph node children sphere");
            node->AddChild(sph);
        }

        for (auto t = childrenTorus.begin(); t != childrenTorus.end(); ++t)
        {
            auto tor = new YafTorus;
            tor->Inner = GetAttribute<float>(*t, "inner", "graph node children torus");
            tor->Outer = GetAttribute<float>(*t, "outer", "graph node children torus");
            tor->Slices = GetAttribute<int>(*t, "slices", "graph node children torus");
            tor->Loops = GetAttribute<int>(*t, "loops", "graph node children torus");
            node->AddChild(tor);
        }

        for (auto n = childrenNodeRef.begin(); n != childrenNodeRef.end(); ++n)
        {
            auto nodeRef = GetAttribute<std::string>(*n, "id", "graph node children noderef");
            node->AddNodeRef(nodeRef);
        }

        for (auto p = childrenPlane.begin(); p != childrenPlane.end(); ++p)
        {
            auto pla = new YafPlane;
            pla->Parts = GetAttribute<int>(*p, "parts", "graph node children plane");
            node->AddChild(pla);
        }

        for (auto p = childrenPatch.begin(); p != childrenPatch.end(); ++p)
        {
            auto pat = new YafPatch;
            pat->Order = GetAttribute<int>(*p, "order", "graph node children patch");
            pat->PartsU = GetAttribute<int>(*p, "partsU", "graph node children patch");
            pat->PartsV = GetAttribute<int>(*p, "partsV", "graph node children patch");
            pat->Compute = YafToOpenGL(YafDrawModeFromString(GetAttribute<std::string>(*p, "compute", "graph node children patch")));

            auto controlPoints = GetAllChildren(*p, "controlpoint");
            for (auto cp = controlPoints.begin(); cp != controlPoints.end(); ++cp)
            {
                auto x = GetAttribute<float>(*cp, "x", "graph node children patch controlpoint");
                auto y = GetAttribute<float>(*cp, "y", "graph node children patch controlpoint");
                auto z = GetAttribute<float>(*cp, "z", "graph node children patch controlpoint");
                pat->ControlPoints.push_back(YafXYZ(x, y, z));
            }

            long long expectedCount = (pat->Order+1) * (pat->Order+1);
            long long actualCount = pat->ControlPoints.size();

            if (expectedCount != actualCount)
                throw YafParsingException("Expected " + std::to_string(expectedCount) + " controlpoints in patch but got " + std::to_string(actualCount));

            node->AddChild(pat);
        }

        for (auto w = childrenWaterline.begin(); w != childrenWaterline.end(); ++w)
        {
            auto wat = new YafWaterline;
            wat->HeightMap = GetAttribute<std::string>(*w, "heightmap", "graph node children waterline");
            wat->TextureMap = GetAttribute<std::string>(*w, "texturemap", "graph node children waterline");
            wat->FragmentShader = GetAttribute<std::string>(*w, "fragmentshader", "graph node children waterline");
            wat->VertexShader = GetAttribute<std::string>(*w, "vertexshader", "graph node children waterline");
            node->AddChild(wat);
        }

        scene->AddNode(node);
    }

    auto rootNode = scene->GetNode(graphRootId);
    if (rootNode)
        scene->SetRootNode(rootNode);
    else
        throw YafParsingException("Root node (" + graphRootId + ") not found.");

    scene->DoPostProcessing();

    return scene;
}

int main(int argc, char* argv[])
{
    if (argc != 2)
    {
        std::cout << "Usage: " + std::string(argv[0]) + " yaf_file_name" << std::endl;
        std::cout << "Press ENTER key to continue." << std::endl;
        std::cin.get();
        return EXIT_SUCCESS;
    }

    YafScene* scene = nullptr;

    try
    {
        scene = ParseYafFile(argv[1]);
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

    auto board = new Board(scene);
    board->FillCells();

    board->AddPiece(Piece(Player::First, scene->GetNode("wPiece1"), YafXY(5, 3)));
    board->AddPiece(Piece(Player::First, scene->GetNode("wPiece2"), YafXY(4, 4)));
    board->AddPiece(Piece(Player::First, scene->GetNode("wPiece3"), YafXY(4, 2)));

    board->AddPiece(Piece(Player::Second, scene->GetNode("bPiece1"), YafXY(3, 3)));
    board->AddPiece(Piece(Player::Second, scene->GetNode("bPiece2"), YafXY(3, 4)));
    board->AddPiece(Piece(Player::Second, scene->GetNode("bPiece3"), YafXY(3, 2)));

    CGFapplication app;

    try
    {
        app.init(&argc, argv);

        app.setScene(scene);
        app.setInterface(new YafInterface());
        scene->initCameras();

        app.run();
    }
    catch (GLexception& ex)
    {
        delete scene;

        std::cerr << "GLexception: " << ex.what() << std::endl;
        std::cout << "Press ENTER key to continue." << std::endl;
        std::cin.get();
        return EXIT_FAILURE;
    }
    catch (std::exception& ex)
    {
        delete scene;

        std::cerr << "Exception: " << ex.what() << std::endl;
        std::cout << "Press ENTER key to continue." << std::endl;
        std::cin.get();
        return EXIT_FAILURE;
    }

    delete scene;

    std::cout << "Press ENTER key to continue." << std::endl;
    std::cin.get();
    return EXIT_SUCCESS;
}
