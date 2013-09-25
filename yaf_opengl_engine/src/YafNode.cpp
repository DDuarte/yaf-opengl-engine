#include "YafNode.h"
#include "YafScene.h"

#include <glm/gtc/matrix_transform.hpp>

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


YafTriangle::YafTriangle(YafXYZ p1, YafXYZ p2, YafXYZ p3)
{
    Point1 = p1;
    Point2 = p2;
    Point3 = p3;
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

void YafRectangle::draw(YafAppearance* app /* = nullptr */)
{
    glNormal3d(0.0, 0.0, 1.0);
    glBegin(GL_QUADS);
    if (app->Texture)
        glTexCoord2f(0.0f, 0.0f);
    glVertex3f(Point1.X, Point1.Y, 0.0f);
    if (app->Texture)
        glTexCoord2f(app->TexLengthS, 0.0f);
    glVertex3f(Point2.X, Point1.Y, 0.0f);
    if (app->Texture)
        glTexCoord2f(app->TexLengthS, app->TexLengthT);
    glVertex3f(Point2.X, Point2.Y, 0.0f);
    if (app->Texture)
        glTexCoord2f(0.0f, app->TexLengthT);
    glVertex3f(Point1.X, Point2.Y, 0.0f);
    glEnd();
}

void YafTriangle::draw(YafAppearance* app /* = nullptr */)
{
    glNormal3d(_normal.X, _normal.Y, _normal.Z);
    glBegin(GL_TRIANGLES);
    if (app->Texture)
        glTexCoord2f(0.0f, 0.0f);
    glVertex3f(Point1.X, Point1.Y, Point1.Z);
    if (app->Texture)
        glTexCoord2f(app->TexLengthS, 0.0f);
    glVertex3f(Point2.X, Point2.Y, Point2.Z);
    if (app->Texture)
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
    glutSolidSphere(Radius, Slices, Stacks);
}


void YafTorus::draw(YafAppearance* /* app /* = nullptr */)
{
    glutSolidTorus(Inner, Outer, Slices, Loops);
}
