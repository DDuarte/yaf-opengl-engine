#ifndef YafInterface_h__
#define YafInterface_h__

#include <CGFinterface.h>
#include <CGFapplication.h>
#include <functional>

#include "YafScene.h"
#include "Main.h"

#define CAMERA_PANEL_ID 3

#define BUFSIZE 256
GLuint selectBuf[BUFSIZE];

class YafInterface : public CGFinterface
{
public:
    virtual void initGUI() override
    {
        _scene = (YafScene*)scene;

        auto lightsPanel = addPanel("Lights");

        for (auto light = _scene->GetLights().begin(); light != _scene->GetLights().end(); ++light)
            addCheckboxToPanel(lightsPanel, const_cast<char*>(light->first.c_str()), reinterpret_cast<int*>(&light->second->Enabled));

        addColumn();

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
    }

    virtual void processGUI(GLUI_Control *ctrl) override
    {
       if (ctrl->user_id == CAMERA_PANEL_ID)
           _scene->activateCamera(ctrl->int_val);
    }

    virtual void processMouse(int button, int state, int x, int y) override
    {
        CGFinterface::processMouse(button,state, x, y);

        if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
            Pick(x, y);
    }

    virtual void processKeyboard(unsigned char key, int x, int y) override
    {
        if (key == 'r')
        {
            auto scene = ParseYafFile(_scene->GetYafName());
            _scene->_backgroundColor = scene->_backgroundColor;
            _scene->_drawMode = scene->_drawMode;
            _scene->_shading = scene->_shading;
            _scene->_cullFace = scene->_cullFace;
            _scene->_cullOrder = scene->_cullOrder;
            _scene->_initialCamera = scene->_initialCamera;
            _scene->_cameras = scene->_cameras;
            _scene->_activeCamera = scene->_activeCamera;
            _scene->_lightDoubleSided = scene->_lightDoubleSided;
            _scene->_lightLocal = scene->_lightLocal;
            _scene->_lightEnabled = scene->_lightEnabled;
            _scene->_lightAmbient = scene->_lightAmbient;
            _scene->_lights = scene->_lights;
            _scene->_textures = scene->_textures;
            _scene->_appearances = scene->_appearances;
            _scene->_animations = scene->_animations;
            _scene->_rootNode = scene->_rootNode;
            _scene->_nodes = scene->_nodes;

            scene->initCameras();
            scene->init();
        }
    }

    void Pick(int x, int y)
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

    void ProcessHits(GLint hits, GLuint* buffer)
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


private:
    YafScene* _scene;
};

#endif // YafInterface_h__
