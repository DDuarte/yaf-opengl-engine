#ifndef YafScene_h__
#define YafScene_h__

#include <string>
#include <map>

#include <CGFscene.h>

#include "YafEnums.h"
#include "YafCamera.h"
#include "YafLight.h"
#include "YafNode.h"
#include "YafAnimation.h"

class YafNode;
class YafTexture;
class YafAppearance;
class Board;

class YafScene : public CGFscene
{
public:
    void OverwriteScene(YafScene* newScene);
    ~YafScene();

    void SetGlobals(YafRGBA bg, YafDrawMode dm, YafShading s, YafCullFace cf, YafCullOrder co);
    void SetLightOptions(bool doubleSided, bool local, bool enabled, YafRGBA ambient);

    YafDrawMode* GetDrawMode() { return &_drawMode; } // can be changed
    int* GetActiveCamera() { return &_activeCamera; } // can be changed

    void DoPostProcessing();

    void SetInitialCamera(const std::string& id) { _initialCamera = _cameras[id]; }
    void SetRootNode(YafNode* node);

    void AddCamera(YafCamera* camera);
    void AddLight(YafLight* light);
    void AddTexture(YafTexture* texture);
    void AddAppearance(YafAppearance* appearance);
    void AddAnimation(YafAnimation* animation);
    void AddNode(YafNode* node);

    YafTexture* GetTexture(const std::string& id) const;
    YafAppearance* GetAppearance(const std::string& id) const;
    YafLight* GetLight(const std::string& id) const;
    YafCamera* GetCamera(const std::string& id) const;
    YafAnimation* GetAnimation(const std::string& id) const;
    YafNode* GetNode(const std::string& id) const;

    std::map<std::string, YafTexture*>& GetTextures() { return _textures; }
    std::map<std::string, YafAppearance*>& GetAppearances() { return _appearances; }
    std::map<std::string, YafLight*>& GetLights() { return _lights; }
    std::map<std::string, YafCamera*>& GetCameras() { return _cameras; }
    std::map<std::string, YafAnimation*>& GetAnimations() { return _animations; }
    std::map<std::string, YafNode*>& GetNodes() { return _nodes; }

    virtual void init() override;
    virtual void display() override;
    virtual void update(unsigned long millis) override;
    virtual void initCameras() override;

    void SetBoard(Board* board) { _board = board; }

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
    int _activeCamera;

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

    //Animations
    std::map<std::string, YafAnimation*> _animations;

    // Graph
    YafNode* _rootNode;
    std::map<std::string, YafNode*> _nodes;

    // Game related
    Board* _board;
};

#endif // YafScene_h__
