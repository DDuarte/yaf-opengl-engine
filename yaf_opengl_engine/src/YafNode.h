#ifndef YafNode_h__
#define YafNode_h__

#include <vector>
#include <string>
#include <windows.h>
#include <gl/GL.h>
#include <gl/freeglut.h>
#include <gl/glui.h>

#include "YafTransform.h"
#include "YafAppearance.h"
#include "NewellsMethod.h"

class YafScene;

class YafChild
{

};

class YafPrimitive : public YafChild
{
public:
    virtual void draw(YafAppearance* app = nullptr) = 0;
};

class YafRectangle : public YafPrimitive
{
public:
    YafXY Point1;
    YafXY Point2;

    virtual void draw(YafAppearance* app = nullptr) override;

};

class YafTriangle : public YafPrimitive
{
public:
    YafTriangle(YafXYZ p1, YafXYZ p2, YafXYZ p3);

    YafXYZ Point1;
    YafXYZ Point2;
    YafXYZ Point3;

    virtual void draw(YafAppearance* app = nullptr) override;

private:
    YafXYZ _normal;

};

class YafCylinder : public YafPrimitive
{
public:
    YafCylinder();
    ~YafCylinder();

    float Base;
    float Top;
    float Height;
    int Slices;
    int Stacks;

    virtual void draw(YafAppearance* app = nullptr) override;

private:
    GLUquadricObj* _quadric;
};

class YafSphere : public YafPrimitive
{
public:
    float Radius;
    int Slices;
    int Stacks;

    virtual void draw(YafAppearance* app = nullptr) override;
};

class YafTorus : public YafPrimitive
{
public:
    float Inner;
    float Outer;
    int Slices;
    int Loops;

    virtual void draw(YafAppearance* app = nullptr) override;
};

class YafNode : public YafChild, public YafElement
{
public:
    YafNode(const std::string& id) : YafElement(id) { }
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
