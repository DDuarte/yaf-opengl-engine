#include "YafScene.h"
#include "YafNode.h"
#include "YafAppearance.h"

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

    for (auto x = _nodes.begin(); x != _nodes.end(); ++x)
        delete x->second;
}

void YafScene::SetGlobals( YafRGBA bg, YafDrawMode dm, YafShading s, YafCullFace cf, YafCullOrder co )
{
    _backgroundColor = bg;
    _drawMode = dm;
    _shading = s;
    _cullFace = cf;
    _cullOrder = co;
}

void YafScene::SetLightOptions( bool doubleSided, bool local, bool enabled, YafRGBA ambient )
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

    glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, _lightDoubleSided);
    glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, _lightLocal);
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, _lightAmbient);

    glEnable(GL_NORMALIZE);

    glShadeModel(YafToOpenGL(_shading));

    glPolygonMode(GL_FRONT_AND_BACK, YafToOpenGL(_drawMode));
    glCullFace(YafToOpenGL(_cullFace));
    glFrontFace(YafToOpenGL(_cullOrder));

    glClearColor(_backgroundColor.R, _backgroundColor.G, _backgroundColor.B, _backgroundColor.A);

    for (auto l = _lights.begin(); l != _lights.end(); ++l)
        l->second->InitLight();

    for (auto t = _textures.begin(); t != _textures.end(); ++t)
        t->second->InitTexture();

    for (auto a = _appearances.begin(); a != _appearances.end(); ++a)
        a->second->InitAppearance();

    setUpdatePeriod(100);
}

void YafScene::display()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    CGFscene::activeCamera->applyView();

    for (auto l = _lights.begin(); l != _lights.end(); ++l)
    {
        if (l->second->Enabled)
            l->second->enable();
        else
            l->second->disable();
        l->second->update();
        l->second->draw();
    }

    glPushMatrix();

    glColor3f(1.0f, 1.0f, 1.0f);

    _rootNode->Draw(_rootNode->GetAppearance());

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

    activateCamera(initialCamera);
}

void YafScene::update( unsigned long millis )
{
    //throw std::logic_error("The method or operation is not implemented.");
}
