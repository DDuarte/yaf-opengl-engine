#include "YafNode.h"
#include "YafScene.h"
#include <iostream>
#include <functional>

#define _USE_MATH_DEFINES // for M_PI
#include <math.h>

#include <GL/glew.h>

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

void YafNode::Update(unsigned long millis)
{
    if (_animation)
    {
        _animation->Update(millis);
        _animation->ApplyAnimation();
    }

    for (auto i = 0u; i < _children.size(); ++i)
        _children[i]->Update(millis);
}

void YafNode::Draw(YafAppearance* app, YafAppearance* appSel)
{
    YafAppearance* appearance = _appearance ? _appearance : app;
    YafAppearance* appearanceSelected = _appearanceSelected ? _appearanceSelected : appSel;

    if (Selected && appearanceSelected)
    {
        appearanceSelected->apply();
        appearance = nullptr;
    }
    else if (appearance)
        appearance->apply();

    glPushMatrix();

    glTranslatef(Position.X, Position.Y, Position.Z);
    glRotatef(Pitch, 1.0f, 0.0f, 0.0f);
    glRotatef(Yaw, 0.0f, 1.0f, 0.0f);
    glRotatef(Roll, 0.0f, 0.0f, 1.0f);
    glScalef(Scale.X, Scale.Y, Scale.Z);

    if (Pickable)
    {
        auto hash = static_cast<GLuint>(std::hash<std::string>()(Id));
        glPushName(hash);
    }

    for (auto i = 0u; i < _children.size(); ++i)
        _children[i]->Draw(appearance, appearanceSelected);

    if (Pickable)
        glPopName();
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

YafTriangle::YafTriangle(YafXYZ<> p1, YafXYZ<> p2, YafXYZ<> p3) : Point1(p1), Point2(p2), Point3(p3)
{
    std::vector<YafXYZ<>> vec;
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

void YafRectangle::Draw(YafAppearance* app /* = nullptr */, YafAppearance* appSel /* = nullptr */)
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

void YafTriangle::Draw(YafAppearance* app /* = nullptr */, YafAppearance* appSel /* = nullptr */)
{
    float a = YafXYZ<>::GetDistance(Point2, Point3);
    float b = YafXYZ<>::GetDistance(Point1, Point3);
    float c = YafXYZ<>::GetDistance(Point1, Point2);

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

void YafCylinder::Draw(YafAppearance* /* app /* = nullptr */, YafAppearance* appSel /* = nullptr */)
{
    glPushMatrix();
    glTranslatef(0.0f, 0.0f, Height);
    gluDisk(_quadric, 0, Top, Slices, Stacks);
    glPopMatrix();

    gluCylinder(_quadric, Base, Top, Height, Slices, Stacks);

    glPushMatrix();
    glRotatef(180.0f, 1.0f, 0.0f, 0.0f);
    gluDisk(_quadric, 0, Base, Slices, Stacks);
    glPopMatrix();
}

void YafSphere::Draw(YafAppearance* /* app /* = nullptr */, YafAppearance* appSel /* = nullptr */)
{
    gluSphere(_quadric, Radius, Slices, Stacks);
}

void YafTorus::Draw(YafAppearance* /* app /* = nullptr */, YafAppearance* appSel /* = nullptr */)
{
    YafXYZ<> vNormal;
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
            vNormal = vNormal.GetNormalized();

            glNormal3f(vNormal.X, vNormal.Y, vNormal.Z);
            glVertex3f(x0 * r, y0 * r, z);

            glTexCoord2f((float)(i + 1) / (float)(Slices), (float)(j) / (float)(Loops));
            vNormal.X = x1 * c;
            vNormal.Y = y1 * c;
            vNormal.Z = z / Loops;
            vNormal = vNormal.GetNormalized();
            glNormal3f(vNormal.X, vNormal.Y, vNormal.Z);
            glVertex3f(x1 * r, y1 * r, z);
        }
        glEnd();
    }
}

GLfloat planeControlPoints[4][3] = { {  0.5f, 0.0f,  0.5f },
                                     { -0.5f, 0.0f,  0.5f },
                                     {  0.5f, 0.0f, -0.5f },
                                     { -0.5f, 0.0f, -0.5f } };

GLfloat planeTexPoints[4][2] = { { 1.0f, 0.0f },
                                 { 0.0f, 0.0f },
                                 { 1.0f, 1.0f },
                                 { 0.0f, 1.0f } };

GLfloat planeNormalPoints[4][3] = { { 0.0f, 1.0, 0.0 },
                                    { 0.0f, 1.0, 0.0 },
                                    { 0.0f, 1.0, 0.0 },
                                    { 0.0f, 1.0, 0.0 } };

void YafPlane::Draw(YafAppearance* app /*= nullptr*/, YafAppearance* appSel /* = nullptr */)
{
    int oldFF;
    glGetIntegerv(GL_FRONT_FACE, &oldFF);
    glFrontFace(GL_CW);

    glMap2f(GL_MAP2_VERTEX_3, 0.0f, 1.0f, 3, 2, 0.0f, 1.0f, 6, 2, &planeControlPoints[0][0]);
    glMap2f(GL_MAP2_NORMAL, 0.0f, 1.0f, 3, 2, 0.0f, 1.0f, 6, 2, &planeNormalPoints[0][0]);
    glMap2f(GL_MAP2_TEXTURE_COORD_2, 0.0f, 1.0f, 2, 2, 0.0f, 1.0f, 4, 2, &planeTexPoints[0][0]);

    glEnable(GL_MAP2_VERTEX_3);
    glEnable(GL_MAP2_NORMAL);
    glEnable(GL_MAP2_TEXTURE_COORD_2);

    glMapGrid2f(Parts, 0.0f, 1.0f, Parts, 0.0f, 1.0f);

    glEvalMesh2(GL_FILL, 0, Parts, 0, Parts);

    glFrontFace(oldFF);
}

void YafPatch::Draw(YafAppearance* app /*= nullptr*/, YafAppearance* appSel /* = nullptr */)
{
    int oldFF;
    glGetIntegerv(GL_FRONT_FACE, &oldFF);
    glFrontFace(GL_CW);

    glMap2f(GL_MAP2_VERTEX_3, 0.0f, 1.0f, 3, Order + 1, 0.0f, 1.0f, 3 * (Order + 1), Order + 1, &ControlPoints[0][0]);
    glMap2f(GL_MAP2_TEXTURE_COORD_2, 0.0f, 1.0f, 2, 2, 0.0f, 1.0f, 4, 2, &planeTexPoints[0][0]);

    glEnable(GL_MAP2_VERTEX_3);
    glEnable(GL_AUTO_NORMAL);
    glEnable(GL_MAP2_TEXTURE_COORD_2);

    glMapGrid2f(PartsU, 0.0f, 1.0f, PartsV, 0.0f, 1.0f);

    glEvalMesh2(Compute, 0, PartsU, 0, PartsV);

    glFrontFace(oldFF);
}

void YafWaterline::Init(YafAppearance* app /*= nullptr*/)
{
    _shader.init(VertexShader.c_str(), FragmentShader.c_str());

    _shader.bind();

    _texture = new CGFtexture(TextureMap);
    _map = new CGFtexture(HeightMap);

    _textureLoc = glGetUniformLocation(_shader.id(), "tex");
    glUniform1i(_textureLoc, 0);

    _mapLoc = glGetUniformLocation(_shader.id(), "map");
    glUniform1i(_mapLoc, 1);

    _shader.unbind();

    _plane.Parts = 100;
}

void YafWaterline::Draw(YafAppearance* app /*= nullptr*/, YafAppearance* appSel /* = nullptr */)
{
    _shader.bind();

    glActiveTexture(GL_TEXTURE0);
    _texture->apply();

    glActiveTexture(GL_TEXTURE1);
    _map->apply();

    glActiveTexture(GL_TEXTURE0);
    _plane.Draw();

    _shader.unbind();
}

YafWaterline::~YafWaterline()
{
    delete _texture;
    delete _map;
}

void YafWaterline::Update(unsigned long millis)
{
    _shader.bind();
    _shader.update((millis % 10000) / 10000.0f);
    _shader.unbind();
}

void YafText::Draw(YafAppearance* app /*= nullptr*/, YafAppearance* appSel /*= nullptr*/)
{
    glDisable(GL_LIGHTING);
    glPushMatrix();
    glColor3f(1.0f, 1.0f, 1.0f);

    auto weird = false;
    for (auto c : Text)
    {
        if (c == '`')
            weird = !weird;
        else
            glutStrokeCharacter(weird ? GLUT_STROKE_MONO_ROMAN : GLUT_STROKE_ROMAN, c);
    }

    glPopMatrix();
    glEnable(GL_LIGHTING);
}
