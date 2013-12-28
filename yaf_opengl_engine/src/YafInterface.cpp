#include "YafInterface.h"

#include <CGFapplication.h>
#include <functional>
#include <iostream>

#include "YafScene.h"
#include "Game.h"
#include "NetworkProlog.h"
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
        printf("Picked ID's: ");
        for (auto i = 0u; i < nselected; i++)
        {
            for (auto n : _scene->GetNodes())
            {
                if ((GLuint) std::hash<std::string>()(n.first) == selected[i])
                {
                    if (_scene->GetBoard()->GetCurrentPlayer() != Board::PlayerFromNode(n.first))
                        break;

                    const Piece* piece = nullptr;

                    for (auto& p : _scene->GetBoard()->GetPieces())
                    {
                        if (p.GetNode() == n.second)
                        {
                            piece = &p;
                            break;
                        }
                    }

                    printf("%s ", n.first.c_str());

                    if (_scene->GetBoard()->GetCurrentState() == GameState::PickSourcePiece)
                    {
                        auto& board = _scene->GetBoard()->GetBoardStack().top();
                        auto x = piece->GetPosition().X;
                        auto y = piece->GetPosition().Y;
                        auto player = Board::PlayerToProlog(_scene->GetBoard()->GetCurrentPlayer());
                        _scene->GetBoard()->GetNetwork()->EnqueueMessage(PrologPredicate::Build("moves_from", '[' + board + ']', x, y, player));
                        auto response = _scene->GetBoard()->GetNetwork()->GetMessage();

                        if (starts_with(response, "moves_from_ok"))
                        {
                            auto firstBracket = response.find_first_of('[');
                            auto lastBracket = response.find_last_of(']');

                            auto moveBlock = response.substr(firstBracket + 1, lastBracket - firstBracket - 1);

                            _scene->GetBoard()->ParsePrologMoves(moveBlock);
                            _scene->GetBoard()->SetCurrentState(GameState::PickDestinationCell);
                            return;
                        }
                    }

                    break;
                }
            }
        }

        printf("\n");
    }
    else
        printf("Nothing selected while picking \n");
}
