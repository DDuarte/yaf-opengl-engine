#include "Game.h"

#include "YafScene.h"
#include "Utilities.h"
#include "NetworkProlog.h"

#include <tuple>
#include <algorithm>

Board::Board(YafScene* scene, NetworkProlog* network) : _scene(scene), _network(network),
_cells(nullptr), _lines(0), _columns(0), _currentPlayer(Player::None), _currentState(GameState::None), _scoreboard(this)
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

void Board::ParsePrologBoard(const std::string& boardStr)
{
    DeassignNodes();

    _boardStrings.push(boardStr);

    auto boardSplit = split_string(boardStr, '[');
    for (auto& str : boardSplit)
    {
        str.erase(std::remove_if(str.begin(), str.end(), [](char c) { return c == ',' || c == '[' || c == ']'; }), str.end());
    }

    for (auto x = 0u; x < boardSplit.size(); ++x)
    {
        for (auto y = 0u; y < boardSplit[x].size(); ++y)
        {
            switch (boardSplit[x][y])
            {
            case 'w':
                AddPiece(Piece(Player::First, YafXY<uint>(x, y)));
                break;
            case 'b':
                AddPiece(Piece(Player::Second, YafXY<uint>(x, y)));
                break;
            case 'o':
                AddPiece(Piece(Player::None, YafXY<uint>(x, y)));
                break;
            }
        }
    }
}

void Board::DeassignNodes()
{
    _whitePieces.push("wPiece1");
    _whitePieces.push("wPiece2");
    _whitePieces.push("wPiece3");
    _blackPieces.push("bPiece1");
    _blackPieces.push("bPiece2");
    _blackPieces.push("bPiece3");

    for (auto& p : _pieces)
        p.SetNode(nullptr);

    _pieces.clear();
}

void Board::AssignNodeForPiece(Piece& piece)
{
    YafNode* node = nullptr;
    YafXY<> pos;

    switch (piece.GetOwner())
    {
        case Player::First:
            node = _scene->GetNode(_whitePieces.front());
            _whitePieces.pop();
            break;
        case Player::Second:
            node = _scene->GetNode(_blackPieces.front());
            _blackPieces.pop();
            break;
        case Player::None:
            node = _scene->GetNode("oPiece");
            break;
        default:
            return;
    }

    pos = YafPieceAnimation::BoardIndexesToXY(piece.GetPosition().X, piece.GetPosition().Y);
    node->Position.X = pos.X;
    node->Position.Y = pos.Y;
    node->Position.Z = 0.0f;
    piece.SetNode(node);
}

Player Board::PlayerFromProlog(const std::string& str)
{
    if (str == "player1" || str == "computer1")
        return Player::First;
    else if (str == "player2" || str == "computer2")
        return Player::Second;
    return Player::None;
}

Player Board::PlayerFromNode(const std::string& id)
{
    if (starts_with(id, "wPiece"))
        return Player::First;
    else if (starts_with(id, "bPiece"))
        return Player::Second;
    return Player::None;
}

std::string Board::PlayerToProlog(Player player)
{
    switch (player)
    {
        case Player::First:
            return "player1";
        case Player::Second:
            return "player2";
        default:
            return "";
    }
}
