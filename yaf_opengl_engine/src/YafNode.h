#ifndef YafNode_h__
#define YafNode_h__

#include <vector>
#include <string>
#include <windows.h>
#include <gl/GL.h>
#include <gl/freeglut.h>
#include <gl/glui.h>
#include <glm/glm.hpp>

#include "YafTransform.h"
#include "YafAppearance.h"
#include "NewellsMethod.h"

class YafScene;

class YafChild
{
public:
    virtual void Draw(YafAppearance* app = nullptr) = 0;

    virtual bool IsCyclic(std::string& which) { return false; }
};

class YafPrimitive : public YafChild
{

};

class YafRectangle : public YafPrimitive
{
public:
    YafXY Point1;
    YafXY Point2;

    virtual void Draw(YafAppearance* app = nullptr) override;

};

class YafTriangle : public YafPrimitive
{
public:
    YafTriangle(YafXYZ p1, YafXYZ p2, YafXYZ p3);

    YafXYZ Point1;
    YafXYZ Point2;
    YafXYZ Point3;

    virtual void Draw(YafAppearance* app = nullptr) override;

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

    virtual void Draw(YafAppearance* app = nullptr) override;

private:
    GLUquadricObj* _quadric;
};

class YafSphere : public YafPrimitive
{
public:
    YafSphere();
    ~YafSphere();

    float Radius;
    int Slices;
    int Stacks;

    virtual void Draw(YafAppearance* app = nullptr) override;

private:
    GLUquadricObj* _quadric;

};

class YafTorus : public YafPrimitive
{
public:
    float Inner;
    float Outer;
    int Slices;
    int Loops;

    virtual void Draw(YafAppearance* app = nullptr) override;
};

class YafNode : public YafChild, public YafElement
{
public:
    YafNode(const std::string& id) : YafElement(id), _processing(false) { }
    void AddTransform(YafTransform* t) { _transforms.push_back(t); }
    void AddChild(YafChild* c) { _children.push_back(c); }
    void SetAppearance(YafAppearance* a) { _appearance = a; }

    void AddNodeRef(const std::string& id) { _refNodes.push_back(id); }

    void DoPostProcessing(YafScene* scene) { MoveRefNodesToChildren(scene); CalculateTransformMatrix(); }

    virtual void Draw(YafAppearance* app = nullptr) override;

    YafAppearance* GetAppearance() const { return _appearance; }

    virtual bool IsCyclic(std::string& which);

private:
    void MoveRefNodesToChildren(YafScene* scene);
    void CalculateTransformMatrix();

    bool _processing;

    std::vector<YafTransform*> _transforms;
    YafAppearance* _appearance; // can be null

    std::vector<YafChild*> _children;

    std::vector<std::string> _refNodes;

    glm::mat4 _m; // partial matrix
};

#endif // YafNode_h__
