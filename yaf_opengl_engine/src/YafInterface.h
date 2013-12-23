#ifndef YafInterface_h__
#define YafInterface_h__

#include <CGFinterface.h>
#include "YafMisc.h"

class YafScene;

class YafInterface : public CGFinterface
{
public:
    YafInterface() : _scene(nullptr), _currentTheme(0) { }
    virtual void initGUI() override;
    virtual void processMouse(int button, int state, int x, int y) override;
    virtual void processKeyboard(unsigned char key, int x, int y) override;
private:
    void Pick(int x, int y);
    void ProcessHits(GLint hits, GLuint* buffer);
    YafScene* _scene;
    uint _currentTheme;
};

#endif // YafInterface_h__
