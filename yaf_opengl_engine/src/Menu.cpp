#include "Menu.h"
#include "YafScene.h"
#include "Game.h"

Menu::Menu(YafScene* scene) : _scene(scene)
{
    _actions["PvP"] = std::mem_fn(&Menu::PlayerVSPlayer);
    _actions["PvEasy"] = std::mem_fn(&Menu::PlayerVSPCEasy);
    _actions["PvNormal"] = std::mem_fn(&Menu::PlayerVSPCNormal);
    _actions["Easy"] = std::mem_fn(&Menu::PCVSPCEasy);
    _actions["Normal"] = std::mem_fn(&Menu::PCVSPCNormal);
    _actions["Exit"] = std::mem_fn(&Menu::Exit);
}

void Menu::Click(std::string buttonName)
{
    auto idx = buttonName.find_last_of('_');
    buttonName = buttonName.substr(idx + 1);

    auto it = _actions.find(buttonName);
    if (it == _actions.end())
        return;

    it->second(*this);
}

void Menu::PlayerVSPlayer()
{
    _board = new Board(_scene, GameMode::PvsP, GameDifficulty::Normal);
}

void Menu::PlayerVSPCEasy()
{
    _board = new Board(_scene, GameMode::PvsPC, GameDifficulty::Easy);
}

void Menu::PlayerVSPCNormal()
{
    _board = new Board(_scene, GameMode::PvsPC, GameDifficulty::Normal);
}

void Menu::PCVSPCEasy()
{
    _board = new Board(_scene, GameMode::PCvsPC, GameDifficulty::Easy);
}

void Menu::PCVSPCNormal()
{
    _board = new Board(_scene, GameMode::PCvsPC, GameDifficulty::Normal);
}

void Menu::Exit()
{
    exit(EXIT_SUCCESS);
}
