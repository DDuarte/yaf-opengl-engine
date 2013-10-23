#include "YafNode.h"
#include "YafScene.h"
#include <iostream>

#define _USE_MATH_DEFINES // for M_PI
#include <math.h>

void YafNode::MoveRefNodesToChildren(YafScene* scene)
{
    for (auto s = _refNodes.begin(); s != _refNodes.end(); ++s)
    {
        if (auto n = scene->GetNode(*s))
            AddChild(n);
        else
            throw YafParsingException("Node " + (*s) + " referenced but not found.");
    }

    _refNodes.clear();
}

void YafNode::CalculateTransformMatrix()
{
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    for (auto t = _transforms.begin(); t != _transforms.end(); ++t)
    {
        (*t)->ApplyTransform();
        delete (*t);
    }

    glGetFloatv(GL_MODELVIEW_MATRIX, &_m[0][0]);

    _transforms.clear();
}

void YafNode::Draw(YafAppearance* app)
{
    if (_appearance) // if we got appearance, override anything that was applied before
        _appearance->apply();
    else if (app) // required (at least) for root node
        app->apply();

    glPushMatrix();
    glMultMatrixf(&_m[0][0]);

    for (auto i = 0u; i < _children.size(); ++i)
        _children[i]->Draw(_appearance ? _appearance : app);

    glPopMatrix();
}

bool YafNode::IsCyclic(std::string& which)
{
    if (_processing)
    {
        which = Id; // error reporting
        return true;
    }

    _processing = true;

    for (auto i = 0u; i < _children.size(); ++i)
        if (_children[i]->IsCyclic(which))
            return true;

    _processing = false;
    return false;
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

    _quadricD1 = gluNewQuadric();
    gluQuadricTexture(_quadricD1, true);

    _quadricD2 = gluNewQuadric();
    gluQuadricTexture(_quadricD2, true);
}

YafCylinder::~YafCylinder()
{
    gluDeleteQuadric(_quadric);
    gluDeleteQuadric(_quadricD1);
    gluDeleteQuadric(_quadricD2);
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

void YafRectangle::Draw(YafAppearance* app /* = nullptr */)
{
    glNormal3f(0.0f, 0.0f, 1.0f);
    glBegin(GL_POLYGON);
    if (app && app->Texture)
        glTexCoord2f(0.0f, 0.0f);
    glVertex2f(Point1.X, Point1.Y);
    if (app && app->Texture)
        glTexCoord2f((Point2.X - Point1.X) / app->TexLengthS, 0.0f);
    glVertex2f(Point2.X, Point1.Y);
    if (app && app->Texture)
        glTexCoord2f((Point2.X - Point1.X) / app->TexLengthS, (Point2.Y - Point1.Y) / app->TexLengthT);
    glVertex2f(Point2.X, Point2.Y);
    if (app && app->Texture)
        glTexCoord2f(0.0f, (Point2.Y - Point1.Y) / app->TexLengthT);
    glVertex2f(Point1.X, Point2.Y);
    glEnd();
}

void YafTriangle::Draw(YafAppearance* app /* = nullptr */)
{
    float a = YafXYZ::GetDistance(Point2, Point3);
    float b = YafXYZ::GetDistance(Point1, Point3);
    float c = YafXYZ::GetDistance(Point1, Point2);

    glNormal3d(_normal.X, _normal.Y, _normal.Z);
    glBegin(GL_TRIANGLES);
    if (app && app->Texture)
        glTexCoord2f(0.0f, 0.0f);
    glVertex3f(Point1.X, Point1.Y, Point1.Z);
    if (app && app->Texture)
        glTexCoord2f(c / app->TexLengthS, 0.0f);
    glVertex3f(Point2.X, Point2.Y, Point2.Z);
    if (app && app->Texture)
    {
        float beta = acos((a*a - b*b + c*c) / (2 * a * c));
        glTexCoord2f((c - a * cos(beta)) / app->TexLengthS, (a * sin(beta)) / app->TexLengthT);
    }
    glVertex3f(Point3.X, Point3.Y, Point3.Z);
    glEnd();
}

void YafCylinder::Draw(YafAppearance* /* app /* = nullptr */)
{
    glPushMatrix();
    glTranslatef(0.0f, 0.0f, Height);
    gluDisk(_quadricD1, 0, Top, Slices, Stacks);
    glPopMatrix();

    gluCylinder(_quadric, Base, Top, Height, Slices, Stacks);

    glPushMatrix();
    glRotatef(180.0f, 1.0f, 0.0f, 0.0f);
    gluDisk(_quadricD2, 0, Base, Slices, Stacks);
    glPopMatrix();
}

void YafSphere::Draw(YafAppearance* /* app /* = nullptr */)
{
    gluSphere(_quadric, Radius, Slices, Stacks);
}

void YafTorus::Draw(YafAppearance* /* app /* = nullptr */)
{
    YafXYZ vNormal;
    double majorStep = 2.0f * M_PI / Slices;
    double minorStep = 2.0f * M_PI / Loops;
    int i, j;

    for (i = 0; i < Slices; i++)
    {
        double a0 = i * majorStep;
        double a1 = a0 + majorStep;
        float x0 = (float)cos(a0);
        float y0 = (float)sin(a0);
        float x1 = (float)cos(a1);
        float y1 = (float)sin(a1);

        glBegin(GL_TRIANGLE_STRIP);
        for (j = 0; j <= Loops; j++)
        {
            double b = j * minorStep;
            float c = (float)cos(b);
            float r = Inner * c + Outer;
            float z = Inner * (float)sin(b);

            glTexCoord2f((float)i / (float)(Slices), (float)(j) / (float)(Loops));
            vNormal.X = x0 * c;
            vNormal.Y = y0 * c;
            vNormal.Z = z / Loops;
            vNormal.GetNormalized();

            glNormal3f(vNormal.X, vNormal.Y, vNormal.Z);
            glVertex3f(x0 * r, y0 * r, z);

            glTexCoord2f((float)(i + 1) / (float)(Slices), (float)(j) / (float)(Loops));
            vNormal.X = x1 * c;
            vNormal.Y = y1 * c;
            vNormal.Z = z / Loops;
            vNormal.GetNormalized();
            glNormal3f(vNormal.X, vNormal.Y, vNormal.Z);
            glVertex3f(x1 * r, y1 * r, z);
        }
        glEnd();
    }
}

void YafNode::Update(unsigned long millis)
{
	if(_animation)
		_animation->Update(millis);

	 for (auto i = 0u; i < _children.size(); ++i)
		 _children[i]->Update(millis);
}
