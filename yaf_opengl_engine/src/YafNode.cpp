#include "YafNode.h"
#include "YafScene.h"

void YafNode::MoveRefNodesToChildren(YafScene* scene)
{
    for (auto s : _refNodes)
        AddChild(scene->GetNode(s));

    _refNodes.clear();
}
