#ifndef YafInterface_h__
#define YafInterface_h__

#include <CGFinterface.h>

#include "YafScene.h"

#define CAMERA_PANEL_ID 3

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

private:
    YafScene* _scene;
};

#endif // YafInterface_h__
