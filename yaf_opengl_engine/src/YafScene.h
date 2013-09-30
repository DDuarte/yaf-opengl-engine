#ifndef YafScene_h__
#define YafScene_h__

#include <string>
#include <map>

#include <CGFscene.h>

#include "YafEnums.h"
#include "YafCamera.h"
#include "YafLight.h"
#include "YafNode.h"

class YafNode;
class YafTexture;
class YafAppearance;

class YafScene : public CGFscene
{
public:
    ~YafScene();

    void SetGlobals(YafRGBA bg, YafDrawMode dm, YafShading s, YafCullFace cf, YafCullOrder co);
    void SetLightOptions(bool doubleSided, bool local, bool enabled, YafRGBA ambient);

    void DoPostProcessing();

    void SetInitialCamera(const std::string& id) { _initialCamera = _cameras[id]; }
    void SetRootNode(const std::string& id) { _rootNode = _nodes[id]; }

    void AddCamera(YafCamera* camera);
    void AddLight(YafLight* light);
    void AddTexture(YafTexture* texture);
    void AddAppearance(YafAppearance* appearance);
    void AddNode(YafNode* node);

    YafTexture* GetTexture(const std::string& id) const;
    YafAppearance* GetAppearance(const std::string& id) const;
    YafLight* GetLight(const std::string& id) const;
    YafNode* GetNode(const std::string& id) const;

    virtual void init() override;
    virtual void display() override;
    virtual void update(unsigned long millis) override;
    virtual void initCameras() override;

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
