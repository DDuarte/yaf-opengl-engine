#include "Game.h"

#include "YafScene.h"
#include "Utilities.h"
#include "NetworkProlog.h"

#include <tuple>
#include <algorithm>

Board::Board(YafScene* scene, GameMode mode, GameDifficulty diff) : _scene(scene), ShowUndo(false),
_cells(nullptr), _lines(0), _columns(0), _currentPlayer(Player::None), _currentState(GameState::None),
_scoreboard(this), _pieceToMove(nullptr), _mode(mode), _diff(diff)
{
    _scene->SetState(State::Game);
    _scene->activateCamera(0);

    if (!_network.Startup())
        throw std::runtime_error("Could not start network.");

    FillCells();

    _network.EnqueueMessage(PrologPredicate::Build("init", ModeToString(mode), DiffToString(diff)));

    auto response = _network.GetMessage();

    if (starts_with(response, "init_invalid"))
    {
        throw std::runtime_error("Received init_invalid.");
    }
    else if (starts_with(response, "init_ok"))
    {
        auto firstBracket = response.find_first_of('[');
        auto lastBracket = response.find_last_of(']');

        auto boardBlock = response.substr(firstBracket + 1, lastBracket - firstBracket - 1);

        auto lastComma = response.find_last_of(',');
        auto lastParens = response.find_last_of(')');

        auto player = response.substr(lastComma + 1, lastParens - lastComma - 1);
        SetCurrentPlayer(Board::PlayerFromProlog(player));

        InitialBoard = boardBlock;
        ParsePrologBoard(boardBlock);
        SetCurrentState(GameState::PickSourcePiece);
    }
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

bool Board::MovePiece(Piece* piece, uint x, uint y)
{
    if (!piece)
        return false;

    if (x > 7 || x < 0 || y > 6 || y < 0) // remove piece
        return true;

    auto anim = new YafPieceAnimation(piece->GetNode()->Id + "anim", piece->GetNode(), piece->GetPosition().X, piece->GetPosition().Y, x, y);
    _scene->AddAnimation(anim);
    piece->GetNode()->SetAnimation(anim);
    piece->SetPosition(x, y);
    return false;
}

void Board::Update(uint millis)
{
    _scoreboard.Update(millis);
}

void Board::ParsePrologMoves(const std::string& movesStr)
{
    ClearSelections();

    auto movesSplit = split_string(movesStr, '[');
    for (auto& str : movesSplit)
    {
        str.erase(std::remove_if(str.begin(), str.end(), [](char c) { return c == ',' || c == '[' || c == ']'; }), str.end());
        uint x = str[0] - '0';
        uint y = str[1] - '0';
        for (auto& p : _pieces)
        {
            p.SetSelected(p.GetPosition().X == x && p.GetPosition().Y == y);
        }

        _cells[x][y]->Selected = true;
    }
}


void Board::ParsePrologMoveTo(const std::string& moveToStr)
{
    auto moveToSplit = split_string(moveToStr, '[');
    std::vector<std::pair<std::pair<YafXY<uint>, Piece*>, YafXY<uint>>> moves;

    for (int i = 0; i < moveToSplit.size(); i += 3)
    {
        auto pair = make_pair(make_pair(YafXY<uint>(moveToSplit[i+1][1] - '0', moveToSplit[i+1][3] - '0'), nullptr),
                                        YafXY<uint>(moveToSplit[i+2][1] - '0', moveToSplit[i+2][3] - '0'));
        moves.push_back(pair);
    }

    for (auto& p : moves)
    {
        p.first.second = const_cast<Piece*>(GetPiece(p.first.first.X, p.first.first.Y));
    }

    bool gameOver = false;

    for (auto& p : moves)
    {
        gameOver |= MovePiece(p.first.second, p.second.X, p.second.Y);
    }

    if (gameOver)
    {
        ResetRound(_currentPlayer);
    }
}

void Board::ParsePrologBoard(const std::string& boardStr)
{
    DeassignNodes();

    AddToBoardStack(boardStr);

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
    {
        p.GetNode()->SetAnimation(nullptr);
        p.SetNode(nullptr);
    }

    _pieces.clear();
}

void Board::AssignNodeForPiece(Piece& piece)
{
    YafNode* node = nullptr;

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

    auto pos = YafPieceAnimation::BoardIndexesToXY(piece.GetPosition().X, piece.GetPosition().Y);
    node->Position.X = pos.X;
    node->Position.Y = pos.Y;
    node->Position.Z = 0.0f;
    node->SetAnimation(nullptr);
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

void Board::DoComputerMove()
{
    if (_mode == GameMode::PvsPC && _currentPlayer == Player::First)
        return;

    if (_boardStrings.empty())
        return;

    auto& board = _boardStrings.top();
    auto player = _currentPlayer == Player::First ? "computer1" : "computer2";
    _network.EnqueueMessage(PrologPredicate::Build("request_computer_move", '[' + board + ']', player));
    auto response = _network.GetMessage();

    if (starts_with(response, "request_computer_move_ok"))
    {
        auto moveStart = response.find_first_of('[');
        auto moveEnd = response.find_last_of("]");
        auto moveBlock = response.substr(moveStart, moveEnd - moveStart + 1);

        // [4,4],[5,4]
        uint x1 = moveBlock[1] - '0';
        uint y1 = moveBlock[3] - '0';
        uint x2 = moveBlock[7] - '0';
        uint y2 = moveBlock[9] - '0';

        SendMoves(x1, y1, x2, y2, false);

        return;
    }
    else if (starts_with(response, "request_computer_move_invalid"))
    {
        std::cerr << "Received " << response << std::endl;
    }
}

void Board::NextPlayer(bool computerMove)
{
    _currentState = GameState::PickSourcePiece;
    _pieceToMove = nullptr;
    ClearSelections();
    _scoreboard.ResetCountdown();

    ShowUndo = true;

    if (_currentPlayer == Player::First)
        _currentPlayer = Player::Second;
    else if (_currentPlayer == Player::Second)
        _currentPlayer = Player::First;

    if (computerMove && _mode != GameMode::PvsP)
        DoComputerMove();
}

void Board::ClearSelections()
{
    for (auto xi = 0u; xi < _lines; ++xi)
    {
        for (auto yi = 0u, realyi = 0u; yi < _columns; ++yi)
        {
            if (xi == 0)
                realyi = yi / 2;
            else
                realyi = yi;

            _cells[xi][realyi]->Selected = false;
        }
    }

    std::for_each(_pieces.begin(), _pieces.end(), [](Piece& p) { p.SetSelected(false); });
}

void Board::ResetRound(Player winner)
{
    ClearSelections();
    _currentState = GameState::PickSourcePiece;
    _currentPlayer = Player::Second;
    _pieceToMove = nullptr;

    _scene->GetNode("oPiece")->Position.Z = -999999.0f; // lol.
    _pieces.erase(std::remove_if(std::begin(_pieces), std::end(_pieces), [](const Piece& p) { return p.GetOwner() == Player::None; }));

    if (winner == Player::First)
        _scoreboard.IncHome();
    else if (winner == Player::Second)
        _scoreboard.IncAway();

    while (!_boardStrings.empty())
        _boardStrings.pop();
    ParsePrologBoard(InitialBoard);
}

void Board::Undo()
{
    ShowUndo = false;

    if (!_boardStrings.empty())
        _boardStrings.pop();

    if (!_boardStrings.empty())
    {
        ParsePrologBoard(_boardStrings.top());
        _boardStrings.pop();
    }

    NextPlayer();
}

void Board::SendMoves(uint x1, uint y1, uint x2, uint y2, bool callNextPlayer)
{
    if (_boardStrings.empty())
        return;

    auto& board = GetBoardStack().top();
    auto player = Board::PlayerToProlog(_currentPlayer);
    _network.EnqueueMessage(PrologPredicate::Build("move_to", '[' + board + ']', x1, y1, x2, y2, player));
    auto response = _network.GetMessage();

    //  move_to_ok([[[4,4],[5,5]],[[4,3],[4,4]],[[3,2],[4,3]]])

    if (starts_with(response, "move_to_ok"))
    {
        auto boardStart = response.find_first_of('[');
        auto boardEnd = response.find("]]");
        auto boardBlock = response.substr(boardStart + 1, boardEnd - boardStart);

        AddToBoardStack(boardBlock);

        auto moveStart = response.find("[[[");
        auto moveEnd = response.find_last_of(']');
        auto moveBlock = response.substr(moveStart + 1, moveEnd - moveStart - 1);

        ParsePrologMoveTo(moveBlock);

        if (callNextPlayer)
            NextPlayer();
        return;
    }
    else if (starts_with(response, "move_to_invalid"))
    {
        std::cerr << "Received " << response << std::endl;
    }
}

void Piece::SetSelected(bool value)
{
    _selected = value;
    if (_node)
        _node->Selected = value;
}
