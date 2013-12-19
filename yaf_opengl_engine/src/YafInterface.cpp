#include "YafInterface.h"

#include <CGFapplication.h>
#include <functional>
#include <iostream>

#include "YafScene.h"
#include "Main.h"

#define CAMERA_PANEL_ID 3
#define YAF_PANEL_ID 4

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

    auto visPanel = addPanel("Visualization");

    auto i = 0;
    auto camerasListBox = addListboxToPanel(visPanel, "Cameras: ", _scene->GetActiveCamera(), CAMERA_PANEL_ID);
    for (auto camera = _scene->GetCameras().begin(); camera != _scene->GetCameras().end(); ++camera, ++i)
        camerasListBox->add_item(i, camera->first.c_str());

    auto drawModeRadioGroup = addRadioGroupToPanel(visPanel, reinterpret_cast<int*>(_scene->GetDrawMode()));
    addRadioButtonToGroup(drawModeRadioGroup, "Fill"); // must match order of YafDrawMode (0, 1, 2)
    addRadioButtonToGroup(drawModeRadioGroup, "Line");
    addRadioButtonToGroup(drawModeRadioGroup, "Point");

    addColumn();

    auto lightsPanel = addPanel("Lights");

    for (auto light = _scene->GetLights().begin(); light != _scene->GetLights().end(); ++light)
        addCheckboxToPanel(lightsPanel, const_cast<char*>(light->first.c_str()), reinterpret_cast<int*>(&light->second->Enabled));

    addColumn();
}

void YafInterface::processGUI(GLUI_Control *ctrl)
{
    if (ctrl->user_id == CAMERA_PANEL_ID)
        _scene->activateCamera(ctrl->int_val);
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
        ReloadYaf(_scene, _scene->GetYafName());
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
