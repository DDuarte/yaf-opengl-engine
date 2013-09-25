#include "YafNode.h"
#include "YafScene.h"



void YafNode::MoveRefNodesToChildren(YafScene* scene)
{
    for (auto s : _refNodes)
        AddChild(scene->GetNode(s));

    _refNodes.clear();
}

void YafRectangle::draw()
{
    glNormal3d(0.0, 0.0, 1.0);
    glBegin(GL_QUADS);            
    glTexCoord2f(0.0f, 0.0f);       
    glVertex3f(Point1.X, Point1.Y, 0.0f); 
    glTexCoord2f(1.0f, 0.0f);       
    glVertex3f(Point2.X, Point1.Y, 0.0f);  
    glTexCoord2f(1.0f, 1.0f);       
    glVertex3f(Point2.X, Point2.Y, 0.0f);   
    glTexCoord2f(0.0f, 1.0f);      
    glVertex3f(Point1.X, Point2.Y, 0.0f);  
    glEnd();
}

void YafTriangle::draw()
{
    glNormal3d(_normal.X, _normal.Y, _normal.Z);
    glBegin(GL_TRIANGLES);            
    glVertex3f(Point1.X, Point1.Y, Point1.Z);
    glVertex3f(Point2.X, Point2.Y, Point2.Z);
    glVertex3f(Point3.X, Point3.Y, Point3.Z);
    glEnd();
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


void YafCylinder::draw()
{
    gluCylinder(_quadric, Base, Top, Height, Slices, Stacks);
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

void YafSphere::draw()
{
    glutSolidSphere(Radius, Slices, Stacks);
}


void YafTorus::draw()
{
    glutSolidTorus(Inner, Outer, Slices, Loops);
}
