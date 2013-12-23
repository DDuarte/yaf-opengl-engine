#include "Game.h"

#include "YafScene.h"
#include "Utilities.h"
#include "NetworkProlog.h"

#include <tuple>
#include <algorithm>

Board::Board(YafScene* scene, NetworkProlog* network) : _scene(scene), _network(network), _cells(nullptr), _lines(0), _columns(0)
{
    _scene->SetBoard(this);
}

void Board::FillCells()
{
    int maxLine = 0;
    int maxCol = 0;

    std::vector<std::tuple<YafNode*, int, int>> cells;
    for (const auto& n : _scene->GetNodes())
    {
        if (starts_with(n.first, "cell") && n.first.size() != 4 /* cell */)
        {
            int line = n.first[n.first.size() - 2] - '0';
            int col = n.first[n.first.size() - 1] - '0';
            cells.push_back(std::make_tuple(n.second, line, col));

            if (line > maxLine)
                maxLine = line;
            if (col > maxCol)
                maxCol = col;
        }
    }

    _lines = maxLine + 1;
    _columns = maxCol + 1;

    _cells = new YafNode**[_lines];
    for (auto i = 0u; i < _lines; ++i)
        _cells[i] = new YafNode*[i == 0 ? _columns / 2 : _columns];

    for (const auto& t : cells)
        _cells[std::get<1>(t)][std::get<2>(t)] = std::get<0>(t);
}

const Piece* Board::GetPiece(uint x, uint y) const
{
    auto it = std::find_if(_pieces.begin(), _pieces.end(), [x, y](const Piece& p)
    {
        auto pos = p.GetPosition();
        return pos.X == x && pos.Y == y;
    });

    if (it == _pieces.end())
        return nullptr;
    else
        return &(*it);
}

void Board::MovePiece(Piece* piece, uint x, uint y) const
{
    auto anim = new YafPieceAnimation(piece->GetNode()->Id + "anim", piece->GetNode(), static_cast<int>(piece->GetPosition().X), static_cast<int>(piece->GetPosition().Y), x, y);
    _scene->AddAnimation(anim);
    piece->GetNode()->SetAnimation(anim);
    piece->SetPosition(x, y);

    _network->EnqueueMessage(PrologPredicate::Build("move", x, y));
}

void Board::Update(uint millis)
{
    _scoreboard.Update(millis);
    for (auto& p : _pieces)
    {
        if (p.GetNode()->Selected)
        {
            for (auto xi = 0u; xi < _lines; ++xi)
            {
                for (auto yi = 0u, realyi = 0u; yi < _columns; ++yi)
                {
                    if (xi == 0) realyi = yi / 2;
                    else
                        realyi = yi;

                    if (_cells[xi][realyi]->Selected)
                    {
                        MovePiece(&p, xi, realyi);
                        p.GetNode()->Selected = false;
                        _cells[xi][realyi]->Selected = false;
                    }
                }
            }
        }
    }
}
