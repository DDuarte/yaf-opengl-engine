#include "YafScene.h"
#include "YafNode.h"
#include "YafAppearance.h"

void YafScene::AddNode(YafNode* node)
{
    _nodes[node->Id] = node;
}

YafNode* YafScene::GetNode(const std::string& id) const
{
    return _nodes.find(id)->second;
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
    return _textures.find(id)->second;
}

YafAppearance* YafScene::GetAppearance(const std::string& id) const
{
    return _appearances.find(id)->second;
}

YafLight* YafScene::GetLight(const std::string& id) const
{
    return _lights.find(id)->second;
}

std::map<std::string, YafNode*>& YafScene::GetNodes()
{
    return _nodes;
}
