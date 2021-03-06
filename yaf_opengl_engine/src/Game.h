#ifndef GAME_H
#define GAME_H

#include <vector>
#include <queue>
#include <stack>

#include "YafMisc.h"
#include "Scoreboard.h"
#include "NetworkProlog.h"

enum class Player
{
    None,
    First,
    Second
};

enum class GameState
{
    None,
    PickSourcePiece,
    PickDestinationCell,
};

enum class GameMode
{
    PvsP,
    PvsPC,
    PCvsPC,
};

enum class GameDifficulty
{
    Easy,
    Normal
};

class YafNode;
class YafScene;

class Piece
{
public:
    Piece(Player owner, YafXY<uint> position) : _owner(owner), _node(nullptr), _position(position), _selected(false) {}

    Player GetOwner() const { return _owner; }
    YafNode* GetNode() const { return _node; }
    YafXY<uint> GetPosition() const { return _position; }
    bool IsSelected() const { return _selected; }

    void SetSelected(bool value);
    void SetPosition(uint x, uint y) { _position.X = x, _position.Y = y; }
    void SetNode(YafNode* node) { _node = node; }

    bool operator==(const Piece& rhs) { return _node ? _node == rhs._node : (_position.X == rhs._position.X && _position.Y == rhs._position.Y); }
private:
    Player _owner;
    YafNode* _node;
    YafXY<uint> _position;
    bool _selected;
};

class Board
{
public:
    Board(YafScene* scene, GameMode mode, GameDifficulty diff);
    ~Board() { for (auto i = 0u; i < _lines; ++i) delete[] _cells[i]; delete[] _cells; }

    void FillCells();

    NetworkProlog* GetNetwork() { return &_network; }

    void AddPiece(Piece& piece) { AssignNodeForPiece(piece); _pieces.push_back(piece); }
    const Piece* GetPiece(uint x, uint y) const;
    bool MovePiece(Piece* piece, uint x, uint y);

    void Update(uint millis);
    void Draw() { _scoreboard.Draw(); }

    void ParsePrologBoard(const std::string& boardStr);
    void ParsePrologMoves(const std::string& movesStr);
    void ParsePrologMoveTo(const std::string& moveToStr);

    void SetCurrentPlayer(Player plr) { _currentPlayer = plr; }
    Player GetCurrentPlayer() const { return _currentPlayer; }

    void SetCurrentState(GameState gs) { _currentState = gs; }
    GameState GetCurrentState() const { return _currentState; }

    void SetPieceToMove(const Piece* piece) { _pieceToMove = piece; }
    const Piece* GetPieceToMove() const { return _pieceToMove; }

    void NextPlayer(bool computerMove = true);

    std::vector<std::string>& GetBoardReplay() { return _boardReplay; }
    const std::stack<std::string>& GetBoardStack() const { return _boardStrings; }
    const std::vector<Piece>& GetPieces() const { return _pieces; }
    void AddToBoardStack(const std::string& str) { _boardStrings.push(str); }

    static Player PlayerFromProlog(const std::string& str);
    static std::string PlayerToProlog(Player player);
    static Player PlayerFromNode(const std::string& id);

    std::string InitialBoard;
    void ResetRound(Player winner);

    bool ShowUndo;
    void Undo();

    GameMode GetMode() const { return _mode; }
    GameDifficulty GetDiff() const { return _diff; }

    void SendMoves(uint x1, uint y1, uint x2, uint y2, bool callNextPlayer = true);
    void DoComputerMove();
private:
    std::vector<Piece> _pieces;
    YafScene* _scene;
    NetworkProlog _network;
    uint _lines;
    uint _columns;
    YafNode*** _cells;
    Scoreboard _scoreboard;
    const Piece* _pieceToMove;
    GameMode _mode;
    GameDifficulty _diff;

    Player _currentPlayer;
    GameState _currentState;

    std::queue<std::string> _whitePieces;
    std::queue<std::string> _blackPieces;

    std::stack<std::string> _boardStrings;
    std::vector<std::string> _boardReplay;

    void AssignNodeForPiece(Piece& piece);
    void DeassignNodes();
    void ClearSelections();

    static std::string ModeToString(GameMode mode)
    {
        switch (mode)
        {
        case GameMode::PvsP:
            return "pVSp";
        case GameMode::PvsPC:
            return "pVSc";
        case GameMode::PCvsPC:
            return "cVSc";
        default:
            return "";
        }
    }

    static std::string DiffToString(GameDifficulty diff)
    {
        switch (diff)
        {
        case GameDifficulty::Easy:
            return "easy";
        case GameDifficulty::Normal:
            return "normal";
        default:
            return "";
        }
    }
};

#endif
