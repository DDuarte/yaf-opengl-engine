#include "YafInterface.h"

#include <CGFapplication.h>
#include <functional>
#include <iostream>

#include "YafScene.h"
#include "Main.h"

#define BUFSIZE 256
GLuint selectBuf[BUFSIZE];

void ReloadYaf(YafScene* scene, const std::string& file)
{
    try
    {
        auto newScene = ParseYafFile(file);
        scene->OverwriteScene(newScene);
    }
    catch (YafParsingException& ex)
    {
        std::cerr << "(ReloadYaf) Exception while parsing caught: " << ex.what() << std::endl;
    }
    catch (GLexception& ex)
    {
        std::cerr << "(ReloadYaf) Glexception while parsing caught: " << ex.what() << std::endl;
    }
}

void YafInterface::initGUI()
{
    _scene = (YafScene*) scene;
}

void YafInterface::processMouse(int button, int state, int x, int y)
{
    CGFinterface::processMouse(button, state, x, y);

    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
        Pick(x, y);
}

void YafInterface::processKeyboard(unsigned char key, int x, int y)
{
    if (key == 'r')
    {
        ReloadYaf(_scene, THEMES[_currentTheme]);
    }
    else if (key == 't')
    {
        _currentTheme = (_currentTheme + 1) % NTHEMES;
        ReloadYaf(_scene, THEMES[_currentTheme]);
    }
}

void YafInterface::Pick(int x, int y)
{
    glSelectBuffer(BUFSIZE, selectBuf);
    glRenderMode(GL_SELECT);

    glInitNames();

    glMatrixMode(GL_PROJECTION);

    glPushMatrix();

    GLfloat projmat[16];
    glGetFloatv(GL_PROJECTION_MATRIX, projmat);

    glLoadIdentity();

    GLint viewport[4];
    glGetIntegerv(GL_VIEWPORT, viewport);

    gluPickMatrix(x, CGFapplication::height - y, 5.0, 5.0, viewport);

    glMultMatrixf(projmat);

    _scene->display();

    glMatrixMode(GL_PROJECTION);
    glPopMatrix();

    glFlush();

    GLint hits = glRenderMode(GL_RENDER);
    ProcessHits(hits, selectBuf);
}

void YafInterface::ProcessHits(GLint hits, GLuint* buffer)
{
    GLuint *ptr = buffer;
    GLuint mindepth = 0xFFFFFFFF;
    GLuint *selected = NULL;
    GLuint nselected;

    // iterate over the list of hits, and choosing the one closer to the viewer (lower depth)
    for (int i = 0; i < hits; i++) {
        int num = *ptr; ptr++;
        GLuint z1 = *ptr; ptr++;
        ptr++;
        if (z1 < mindepth && num>0) {
            mindepth = z1;
            selected = ptr;
            nselected = num;
        }
        for (int j = 0; j < num; j++)
            ptr++;
    }

    // if there were hits, the one selected is in "selected", and it consist of nselected "names" (integer ID's)
    if (selected != NULL)
    {
        // this should be replaced by code handling the picked object's ID's (stored in "selected"),
        // possibly invoking a method on the scene class and passing "selected" and "nselected"
        printf("Picked ID's: ");
        for (auto i = 0u; i < nselected; i++)
            for (auto n : _scene->GetNodes())
                if ((GLuint) std::hash<std::string>()(n.first) == selected[i])
                {
                    printf("%s ", n.first.c_str());
                    n.second->Selected = !n.second->Selected;
                    break;
                }

        printf("\n");
    }
    else
        printf("Nothing selected while picking \n");
}
