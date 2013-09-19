#ifndef YafNode_h__
#define YafNode_h__

#include <vector>
#include <string>

#include "YafTransform.h"
#include "YafAppearance.h"

class YafScene;

class YafChild
{

};

class YafPrimitive : public YafChild
{

};

class YafRectangle : public YafPrimitive
{
public:
    YafXY Point1;
    YafXY Point2;
};

class YafTriangle : public YafPrimitive
{
public:
    YafXYZ Point1;
    YafXYZ Point2;
    YafXYZ Point3;
};

class YafCylinder : public YafPrimitive
{
public:
    float Base;
    float Top;
    float Height;
    int Slices;
    int Stacks;
};

class YafSphere : public YafPrimitive
{
public:
    float Radius;
    int Slices;
    int Stacks;
};

class YafTorus : public YafPrimitive
{
public:
    float Inner;
    float Outer;
    int Slices;
    int Loops;
};

class YafNode : public YafChild, public YafElement
{
public:
    void AddTransform(YafTransform* t) { _transforms.push_back(t); }
    void AddChild(YafChild* c) { _children.push_back(c); }
    void SetAppearance(YafAppearance* a) { _appearance = a; }

    void AddNodeRef(const std::string& id) { _refNodes.push_back(id); }

    void MoveRefNodesToChildren(YafScene* scene);

private:
    std::vector<YafTransform*> _transforms;
    YafAppearance* _appearance; // can be null

    std::vector<YafChild*> _children;

    std::vector<std::string> _refNodes;
};

#endif // YafNode_h__
