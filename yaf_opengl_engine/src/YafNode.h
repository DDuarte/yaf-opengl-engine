#ifndef YafNode_h__
#define YafNode_h__

#include <vector>
#include <string>
#include <windows.h>
#include <gl/glew.h>
#include <gl/freeglut.h>
#include <gl/glui.h>
#include <CGFshader.h>

#include "YafAppearance.h"
#include "NewellsMethod.h"


class YafScene;
class YafAnimation;

class YafChild
{
public:
    virtual void Init(YafAppearance* app = nullptr) { };

    virtual void Draw(YafAppearance* app = nullptr) = 0;

    virtual void Update(unsigned long millis) { }

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
    GLUquadricObj* _quadricD1;
    GLUquadricObj* _quadricD2;
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

class YafPlane : public YafPrimitive
{
public:
    int Parts;

    virtual void Draw(YafAppearance* app = nullptr) override;
};

class YafPatch : public YafPrimitive
{
public:
    int Order;
    int PartsU;
    int PartsV;
    GLenum Compute;

    std::vector<YafXYZ> ControlPoints;

    virtual void Draw(YafAppearance* app = nullptr) override;
};

class YafWaterline : public YafPrimitive
{
public:
    ~YafWaterline();
    std::string HeightMap; // filename
    std::string TextureMap; // filename
    std::string FragmentShader; // filename
    std::string VertexShader; // filename

    virtual void Init(YafAppearance* app = nullptr) override;
    virtual void Update(unsigned long millis) override; // in seconds
    virtual void Draw(YafAppearance* app = nullptr) override;
private:
    CGFshader _shader;
    CGFtexture* _texture;
    CGFtexture* _map;
    YafTexture* _heightMap;
    GLint _textureLoc;
    GLint _mapLoc;
    YafPlane _plane;
};

class YafNode : public YafChild, public YafElement
{
public:
    YafNode(const std::string& id) : YafElement(id), _processing(false), Selected(false) { }

    void AddChild(YafChild* c) { _children.push_back(c); }
    void SetAppearance(YafAppearance* a) { _appearance = a; }

    void SetAnimation(YafAnimation* a) { _animation = a; }

    void AddNodeRef(const std::string& id) { _refNodes.push_back(id); }

    void DoPostProcessing(YafScene* scene) { MoveRefNodesToChildren(scene); }

    virtual void Draw(YafAppearance* app = nullptr) override;

    YafAppearance* GetAppearance() const { return _appearance; }

    YafAnimation* GetAnimation() const { return _animation; }

    virtual bool IsCyclic(std::string& which);

    virtual void Update(unsigned long millis) override;

    bool Pickable;
    bool Selected;

    YafXYZ Position;
    float Pitch;
    float Yaw;
    float Roll;
    YafXYZ Scale;
private:
    void MoveRefNodesToChildren(YafScene* scene);

    bool _processing; // used in cycle detection

    YafAppearance* _appearance; // can be null

    std::vector<YafChild*> _children;

    std::vector<std::string> _refNodes; // cache, cleared after parsing, stored in _children

    YafAnimation* _animation;
};

#endif // YafNode_h__
