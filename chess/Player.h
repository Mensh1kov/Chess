#pragma once
#include "ChessPieces.h"
#include <set>
enum StatusPlayer {winner, draw, loser, can_play, uncertainties};
using SetPieces = std::set<ChessPiece * >;

class Player {
    std::string name;
    StatusPlayer status = can_play;
    Color color;
    bool can_move = false;
    SetPieces pieces = {};
public:
    Player(std::string name, Color color) : name(std::move(name)), color(color) {}
    ~Player() {
        for (auto i: pieces) {
            delete i;
            i = nullptr;
        }
    }

    std::string GetName() const { return name; }
    const SetPieces & GetPieces() const { return pieces; }
    StatusPlayer GetStatus() const { return status; }
    bool FindPiece(ChessPiece * piece) const { return pieces.find(piece) != pieces.end(); }
    bool GetCanMove() const { return can_move; }
    Color GetColor() const { return color; }

    void EditPieces(SetPieces & new_pieces) { pieces = new_pieces; }
    void EditCanMove(bool new_value) { can_move = new_value; }
    void EditStatus(StatusPlayer new_status) { status = new_status; }
    void AddPiece(ChessPiece * piece) { pieces.insert(piece); }
    void PopPiece(ChessPiece * piece) { pieces.erase(piece); }
};