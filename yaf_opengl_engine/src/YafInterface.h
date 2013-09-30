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

        GLUI_Panel* lightsPanel = addPanel("Lights");

        for (auto light = _scene->GetLights().begin(); light != _scene->GetLights().end(); ++light)
        {
            addCheckboxToPanel(lightsPanel, const_cast<char*>(light->first.c_str()), reinterpret_cast<int*>(&light->second->Enabled));

            addColumn();
        }
    }

    virtual void processGUI(GLUI_Control *ctrl) override
    {
       // throw std::logic_error("The method or operation is not implemented.");
    }

private:
    YafScene* _scene;
};

#endif // YafInterface_h__
