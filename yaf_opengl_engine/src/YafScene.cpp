#include "YafScene.h"
#include "YafNode.h"
#include "YafAppearance.h"

#include <iostream>

void YafScene::AddNode(YafNode* node)
{
    _nodes[node->Id] = node;
}

YafNode* YafScene::GetNode(const std::string& id) const
{
    auto n = _nodes.find(id);
    return n != _nodes.end() ? n->second : nullptr;
}

void YafScene::AddAppearance(YafAppearance* appearance)
{
    _appearances[appearance->Id] = appearance;
}

void YafScene::AddTexture(YafTexture* texture)
{
    _textures[texture->Id] = texture;
}

void YafScene::AddLight(YafLight* light)
{
    _lights[light->Id] = light;
}

void YafScene::AddCamera(YafCamera* camera)
{
    _cameras[camera->Id] = camera;
}

YafTexture* YafScene::GetTexture(const std::string& id) const
{
    auto t = _textures.find(id);
    return t != _textures.end() ? t->second : nullptr;
}

YafAppearance* YafScene::GetAppearance(const std::string& id) const
{
    auto a = _appearances.find(id);
    return a != _appearances.end() ? a->second : nullptr;
}

YafLight* YafScene::GetLight(const std::string& id) const
{
    auto l = _lights.find(id);
    return l != _lights.end() ? l->second : nullptr;
}

YafCamera* YafScene::GetCamera(const std::string& id) const
{
    auto c = _cameras.find(id);
    return c != _cameras.end() ? c->second : nullptr;
}

void YafScene::DoPostProcessing()
{
    for (auto n = _nodes.begin(); n != _nodes.end(); ++n)
        n->second->DoPostProcessing(this);

    std::string which;
    if (_rootNode->IsCyclic(which))
        throw YafParsingException("Cycle in node " + which + " detected.");
}

YafScene::~YafScene()
{
    for (auto x = _cameras.begin(); x != _cameras.end(); ++x)
        delete x->second;

    for (auto x = _lights.begin(); x != _lights.end(); ++x)
        delete x->second;

    for (auto x = _textures.begin(); x != _textures.end(); ++x)
        delete x->second;

    for (auto x = _appearances.begin(); x != _appearances.end(); ++x)
        delete x->second;

    for (auto x = _animations.begin(); x != _animations.end(); ++x)
        delete x->second;

    for (auto x = _nodes.begin(); x != _nodes.end(); ++x)
        delete x->second;
}

void YafScene::SetGlobals(YafRGBA bg, YafDrawMode dm, YafShading s, YafCullFace cf, YafCullOrder co)
{
    _backgroundColor = bg;
    _drawMode = dm;
    _shading = s;
    _cullFace = cf;
    _cullOrder = co;
}

void YafScene::SetLightOptions(bool doubleSided, bool local, bool enabled, YafRGBA ambient)
{
    _lightDoubleSided = doubleSided;
    _lightLocal = local;
    _lightEnabled = enabled;
    _lightAmbient = ambient;
}

void YafScene::init()
{
    if (_lightEnabled)
        glEnable(GL_LIGHTING);

    std::cout << reinterpret_cast<const char *>(glGetString(GL_VERSION)) << std::endl;

    glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, _lightDoubleSided);
    glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, _lightLocal);
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, _lightAmbient);

    glEnable(GL_NORMALIZE);

    glShadeModel(YafToOpenGL(_shading));

    glCullFace(YafToOpenGL(_cullFace));
    glFrontFace(YafToOpenGL(_cullOrder));

    glClearColor(_backgroundColor.R, _backgroundColor.G, _backgroundColor.B, _backgroundColor.A);

    for (auto l = _lights.begin(); l != _lights.end(); ++l)
        l->second->InitLight();

    for (auto t = _textures.begin(); t != _textures.end(); ++t)
        t->second->InitTexture();

    for (auto a = _appearances.begin(); a != _appearances.end(); ++a)
        a->second->InitAppearance();

    _rootNode->Init(new YafAppearance());

    setUpdatePeriod(100);
}

void YafScene::display()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glPolygonMode(GL_FRONT_AND_BACK, YafToOpenGL(_drawMode)); // can be changed in interface

    CGFscene::activeCamera->applyView();

    for (auto l = _lights.begin(); l != _lights.end(); ++l)
    {
        if (l->second->Enabled)
        {
            l->second->enable();
            l->second->draw(); // draw calls update
        }
        else
        {
            l->second->disable();
            l->second->update();
        }
    }

    glPushMatrix();
    glPushName(-1); // default name

    glColor3f(1.0f, 1.0f, 1.0f);
    axis.draw();

    _rootNode->Draw();

    glPopMatrix();

    glutSwapBuffers();
}

void YafScene::initCameras()
{
    int i = 0;
    int initialCamera = 0;
    for (auto cam = _cameras.begin(); cam != _cameras.end(); ++cam)
    {
        scene_cameras.push_back(cam->second);
        if (cam->second->Id == _initialCamera->Id)
            initialCamera = i;
        i++;
    }

    _activeCamera = initialCamera;
    activateCamera(initialCamera);
}

void YafScene::update(unsigned long millis)
{
    _rootNode->Update(millis);
}

void YafScene::SetRootNode(YafNode* node)
{
    _rootNode = node;
    if (!_rootNode->GetAppearance())
        _rootNode->SetAppearance(new YafAppearance());
}

void YafScene::AddAnimation(YafAnimation* animation)
{
    _animations[animation->Id] = animation;
}

 YafAnimation* YafScene::GetAnimation(const std::string& id) const
 {
    auto a = _animations.find(id);
    return a != _animations.end() ? a->second : nullptr;
 }

 void YafScene::OverwriteScene(YafScene* newScene)
 {
     /*
     for (auto x = _cameras.begin(); x != _cameras.end(); ++x)
         delete x->second;

     for (auto x = _lights.begin(); x != _lights.end(); ++x)
         delete x->second;

     for (auto x = _textures.begin(); x != _textures.end(); ++x)
         delete x->second;

     for (auto x = _appearances.begin(); x != _appearances.end(); ++x)
         delete x->second;

     for (auto x = _animations.begin(); x != _animations.end(); ++x)
         delete x->second;

     for (auto x = _nodes.begin(); x != _nodes.end(); ++x)
         delete x->second; */

     _backgroundColor = newScene->_backgroundColor;
     _drawMode = newScene->_drawMode;
     _shading = newScene->_shading;
     _cullFace = newScene->_cullFace;
     _cullOrder = newScene->_cullOrder;
     _initialCamera = newScene->_initialCamera;
     _cameras = newScene->_cameras;
     _activeCamera = newScene->_activeCamera;
     _lightDoubleSided = newScene->_lightDoubleSided;
     _lightLocal = newScene->_lightLocal;
     _lightEnabled = newScene->_lightEnabled;
     _lightAmbient = newScene->_lightAmbient;
     _lights = newScene->_lights;
     _textures = newScene->_textures;
     _appearances = newScene->_appearances;
     _animations = newScene->_animations;
     _rootNode = newScene->_rootNode;
     _nodes = newScene->_nodes;

     newScene->initCameras();
     newScene->init();
 }
