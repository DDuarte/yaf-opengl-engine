#ifndef YafInterface_h__
#define YafInterface_h__

#include <CGFinterface.h>

#include "YafScene.h"

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

        auto drawModeRadioGroup = addRadioGroupToPanel(visPanel, reinterpret_cast<int*>(_scene->GetDrawMode()));
        addRadioButtonToGroup(drawModeRadioGroup, "Fill"); // must match order of YafDrawMode (0, 1, 2)
        addRadioButtonToGroup(drawModeRadioGroup, "Line");
        addRadioButtonToGroup(drawModeRadioGroup, "Point");
    }

    virtual void processGUI(GLUI_Control *ctrl) override
    {
       // throw std::logic_error("The method or operation is not implemented.");
    }

private:
    YafScene* _scene;
};

#endif // YafInterface_h__
