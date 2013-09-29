#include "YafNode.h"
#include "YafScene.h"

#include <glm/gtc/type_ptr.hpp>

void YafNode::MoveRefNodesToChildren(YafScene* scene)
{
    for (auto s = _refNodes.begin(); s != _refNodes.end(); ++s)
        AddChild(scene->GetNode(*s));

    _refNodes.clear();
}

void YafNode::CalculateTransformMatrix()
{
    for (auto t = _transforms.begin(); t != _transforms.end(); ++t)
    {
        (*t)->ApplyTransform(_m);
        delete (*t);
    }

    _transforms.clear();
}

void YafNode::draw(YafAppearance* app)
{
    glPushMatrix();
    glMultMatrixf(glm::value_ptr(_m));
    for (auto i = 0u; i < _children.size(); ++i)
        _children[i]->draw(app);
    glPopMatrix();
}

YafTriangle::YafTriangle(YafXYZ p1, YafXYZ p2, YafXYZ p3) : Point1(p1), Point2(p2), Point3(p3)
{
    std::vector<YafXYZ> vec;
    vec.push_back(Point1);
    vec.push_back(Point2);
    vec.push_back(Point3);
    _normal = CalculateSurfaceNormal(vec);
}

YafCylinder::YafCylinder()
{
    _quadric = gluNewQuadric();
    gluQuadricTexture(_quadric, true);
}

YafCylinder::~YafCylinder()
{
    gluDeleteQuadric(_quadric);
}

YafSphere::YafSphere()
{
    _quadric = gluNewQuadric();
    gluQuadricTexture(_quadric, true);
}

YafSphere::~YafSphere()
{
    gluDeleteQuadric(_quadric);
}

void YafRectangle::draw(YafAppearance* app /* = nullptr */)
{
    glNormal3f(0.0f, 0.0f, 1.0f);
    glBegin(GL_QUADS);
    if (app && app->Texture)
        glTexCoord2f(0.0f, 0.0f);
    glVertex3f(Point1.X, Point1.Y, 0.0f);
    if (app && app->Texture)
        glTexCoord2f(app->TexLengthS, 0.0f);
    glVertex3f(Point2.X, Point1.Y, 0.0f);
    if (app && app->Texture)
        glTexCoord2f(app->TexLengthS, app->TexLengthT);
    glVertex3f(Point2.X, Point2.Y, 0.0f);
    if (app && app->Texture)
        glTexCoord2f(0.0f, app->TexLengthT);
    glVertex3f(Point1.X, Point2.Y, 0.0f);
    glEnd();
}

void YafTriangle::draw(YafAppearance* app /* = nullptr */)
{
    glNormal3d(_normal.X, _normal.Y, _normal.Z);
    glBegin(GL_TRIANGLES);
    if (app && app->Texture)
        glTexCoord2f(0.0f, 0.0f);
    glVertex3f(Point1.X, Point1.Y, Point1.Z);
    if (app && app->Texture)
        glTexCoord2f(app->TexLengthS, 0.0f);
    glVertex3f(Point2.X, Point2.Y, Point2.Z);
    if (app && app->Texture)
        glTexCoord2f(0.5f * app->TexLengthS, app->TexLengthT);
    glVertex3f(Point3.X, Point3.Y, Point3.Z);
    glEnd();
}

void YafCylinder::draw(YafAppearance* /* app /* = nullptr */)
{
    gluCylinder(_quadric, Base, Top, Height, Slices, Stacks);
}

void YafSphere::draw(YafAppearance* /* app /* = nullptr */)
{
    gluSphere(_quadric, Radius, Slices, Stacks);
}

void YafTorus::draw(YafAppearance* /* app /* = nullptr */)
{
    // TODO: rewrite this, do not use glut
    glutSolidTorus(Inner, Outer, Slices, Loops);
}
