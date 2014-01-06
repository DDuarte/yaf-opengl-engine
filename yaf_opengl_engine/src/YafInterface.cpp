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
        scene->GetBoard()->FillCells();
        scene->GetBoard()->ParsePrologBoard(scene->GetBoard()->GetBoardStack().top());
    }
    catch (YafParsingException& ex)
    {
        std::cerr << "(ReloadYaf) Exception while parsing caught: " << ex.what() << std::endl;
    }
    catch (GLexception& ex)
    {
        std::cerr << "(ReloadYaf) GLexception while parsing caught: " << ex.what() << std::endl;
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
    {
        if (_scene->GetState() == State::Game)
        {
            if (_scene->GetBoard()->ShowUndo)
            {
                if (x >= 10.0f && x <= 65.0f && (CGFapplication::height - y) >= 10.0f && (CGFapplication::height - y) <= 65.0f)
                {
                    _scene->GetBoard()->Undo();
                    return;
                }
            }

            if (_scene->GetBoard()->GetMode() == GameMode::PCvsPC)
            {
                _scene->GetBoard()->DoComputerMove();
                _scene->GetBoard()->NextPlayer(false);
                return;
            }
        }
        else if (_scene->GetState() == State::GameReplay)
        {
            if (_scene->GetBoard()->GetBoardReplay().empty())
                _scene->GetBoard()->ResetRound(Player::None);
            else
            {
                _scene->GetBoard()->ParsePrologBoard(_scene->GetBoard()->GetBoardReplay().front());
                _scene->GetBoard()->GetBoardReplay().erase(_scene->GetBoard()->GetBoardReplay().begin());
            }

            return;
        }

        Pick(x, y);
    }
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

    _scene->Picking = true;
    _scene->display();
    _scene->Picking = false;

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
    for (int i = 0; i < hits; i++)
    {
        int num = *ptr; ptr++;
        GLuint z1 = *ptr; ptr++;
        ptr++;
        if (z1 < mindepth && num > 0)
        {
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
        for (auto i = 0u; i < nselected; i++)
        {
            for (auto n : _scene->GetNodes())
            {
                if ((GLuint) std::hash<std::string>()(n.first) == selected[i])
                {
                    printf("%s ", n.first.c_str());
                    if (_scene->GetState() == State::Menu)
                    {
                        if (starts_with(n.first, "button_"))
                            _scene->GetMenu().Click(n.first);
                        return;
                    }

                    if (_scene->GetBoard()->GetMode() == GameMode::PCvsPC)
                        return;

                    if (_scene->GetBoard()->GetCurrentPlayer() != Player::First &&
                        _scene->GetBoard()->GetMode() == GameMode::PvsPC)
                        return;

                    if (_scene->GetBoard()->GetCurrentState() == GameState::PickSourcePiece &&
                        _scene->GetBoard()->GetCurrentPlayer() != Board::PlayerFromNode(n.first))
                        return;

                    const Piece* piece = nullptr;

                    auto isCell = starts_with(n.first, "cell");
                    if (!isCell)
                    {
                        for (auto& p : _scene->GetBoard()->GetPieces())
                        {
                            if (p.GetNode() == n.second)
                            {
                                piece = &p;
                                break;
                            }
                        }
                    }

                    printf("%s ", n.first.c_str());

                    if (_scene->GetBoard()->GetCurrentState() == GameState::PickSourcePiece && !isCell)
                    {
                        if (_scene->GetBoard()->GetBoardStack().empty())
                            break;

                        auto& board = _scene->GetBoard()->GetBoardStack().top();
                        auto player = Board::PlayerToProlog(_scene->GetBoard()->GetCurrentPlayer());

                        auto x = piece->GetPosition().X;
                        auto y = piece->GetPosition().Y;
                        _scene->GetBoard()->GetNetwork()->EnqueueMessage(PrologPredicate::Build("moves_from", '[' + board + ']', x, y, player));
                        _scene->GetBoard()->SetPieceToMove(piece);
                        auto response = _scene->GetBoard()->GetNetwork()->GetMessage();

                        if (starts_with(response, "moves_from_ok"))
                        {
                            auto firstBracket = response.find_first_of('[');
                            auto lastBracket = response.find_last_of(']');

                            auto moveBlock = response.substr(firstBracket + 1, lastBracket - firstBracket - 1);

                            _scene->GetBoard()->ParsePrologMoves(moveBlock);
                            _scene->GetBoard()->SetCurrentState(GameState::PickDestinationCell);
                            _scene->GetBoard()->ShowUndo = false;
                            return;
                        }
                        else if (starts_with(response, "moves_from_invalid"))
                        {
                            std::cerr << "Received " << response << std::endl;
                        }

                        // request_computer_move(Board, Player)
                    }
                    else if (_scene->GetBoard()->GetCurrentState() == GameState::PickDestinationCell)
                    {
                        auto pieceToMove = _scene->GetBoard()->GetPieceToMove();
                        auto x1 = pieceToMove->GetPosition().X;
                        auto y1 = pieceToMove->GetPosition().Y;
                        uint x2, y2;
                        if (isCell) // selection is cell
                        {
                            x2 = n.first[4] - '0'; // cellXY
                            y2 = n.first[5] - '0'; // cellXY
                        }
                        else // selection is piece
                        {
                            x2 = piece->GetPosition().X;
                            y2 = piece->GetPosition().Y;
                        }

                        _scene->GetBoard()->SendMoves(x1, y1, x2, y2);
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
