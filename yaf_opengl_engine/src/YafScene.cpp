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

std::map<std::string, YafNode*>& YafScene::GetNodes()
{
    return _nodes;
}

void YafScene::DoPostProcessing()
{
    for (auto n = _nodes.begin(); n != _nodes.end(); ++n)
        n->second->DoPostProcessing(this);
}
