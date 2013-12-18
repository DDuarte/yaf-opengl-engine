#ifndef GAME_H
#define GAME_H

#include <vector>

#include "YafMisc.h"
#include "Scoreboard.h"

enum class Player
{
    None,
    First,
    Second
};

class YafNode;
class YafScene;
class NetworkProlog;

class Piece
{
public:
    Piece(Player owner, YafNode* node, YafXY position) : _owner(owner), _node(node), _position(position), _selected(false) {}

    Player GetOwner() const { return _owner; }
    YafNode* GetNode() const { return _node; }
    YafXY GetPosition() const { return _position; }
    bool IsSelected() const { return _selected; }

    void SetSelected(bool value) { _selected = _selected; }
    void SetPosition(uint x, uint y) { _position.X = static_cast<float>(x), _position.Y = static_cast<float>(y); }

private:
    Player _owner;
    YafNode* _node;
    YafXY _position;
    bool _selected;
};

class Board
{
public:
    Board(YafScene* scene, NetworkProlog* network);
    ~Board() { for (auto i = 0u; i < _lines; ++i) delete[] _cells[i]; delete[] _cells; }

    void FillCells();

    void AddPiece(Piece& piece) { _pieces.push_back(piece); }
    const Piece* GetPiece(uint x, uint y) const;
    void MovePiece(Piece* piece, uint x, uint y) const;

    void Update(uint millis);
    void Draw() {_scoreboard.Draw(); };
private:
    std::vector<Piece> _pieces;
    YafScene* _scene;
    NetworkProlog* _network;
    uint _lines;
    uint _columns;
    YafNode*** _cells;
    Scoreboard _scoreboard;
};

#endif
