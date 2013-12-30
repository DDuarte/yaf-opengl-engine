#ifndef MENU_H
#define MENU_H

#include <string>
#include <unordered_map>
#include <functional>

class Board;
class YafScene;

class Menu
{
public:
    Menu(YafScene* scene);
    Board* GetBoard() const { return _board; }
    void Click(std::string buttonName);
private:
    Board* _board = nullptr;
    YafScene* _scene;
    std::unordered_map<std::string, std::function<void(Menu&)>> _actions;
    void PlayerVSPlayer();
    void PlayerVSPCEasy();
    void PlayerVSPCNormal();
    void PCVSPCEasy();
    void PCVSPCNormal();
    void Exit();
};

#endif // MENU_H
