#ifndef YafScene_h__
#define YafScene_h__

#include <string>
#include <map>

#include "YafEnums.h"

#include "YafCamera.h"
#include "YafLight.h"
#include "YafNode.h"
#include "YafAppearance.h"

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

#endif // YafScene_h__
