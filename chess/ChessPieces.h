#pragma once
#include <map>

enum Orientation {first_player = 1, second_player = -1};
enum Piece {king, queen, rook, bishop, knight, pawn};
enum Color {white, black};
using Pos = std::pair<short int, short int>;
using MapMove = std::map<Pos, Pos>;

class ChessPiece {
protected:
    Pos position;
    Piece level;
    Color color;
    MapMove available_moves = {};
    bool walked = false;
public:
    ChessPiece() {}
    ChessPiece(Pos position, Piece level, Color color) : position(std::move(position)), level(level), color(color) {}
    const Pos & GetPosition() const { return position; }
    const Piece & GetLevel() const { return level; }
    Color GetColor() const { return color; }
    const MapMove & GetMove() const { return available_moves; }
    bool GetWalked() const { return walked; }
    bool FindMove(Pos key) const { return available_moves.find(key) != available_moves.end(); }

    void EditPosition(Pos new_pos) { position = new_pos; }
    void EditMove(MapMove & new_available_moves) { available_moves = new_available_moves; }
    void EditWalked(bool new_value) { walked = new_value; }
};
class King : public ChessPiece { // король
    bool check = false;
public:
    King(Pos position, Color color) {
        this->position = position;
        this->level = king;
        this->color = color;
    }
    bool GetCheck() const { return check; }

    void EditCheck(bool new_check) { check = new_check; }
};
class Pawn : public ChessPiece { // пешка
    Orientation orientation;
public:
    Pawn(Pos position, Color color, Orientation orientation) {
        this->orientation = orientation;
        this->position = position;
        this->level = pawn;
        this->color = color;
    }
    const Orientation & GetOrientation() { return orientation; }
};

/*Шахматы — chess Король — king Ферзь — queen
Ладья — rook Слон — bishop Конь — knight
Пешка — pawn Chess board — шахматная доска
Piece — фигура Ranks — горизонтали Files — вертикали
Diagonals — диагонали Square — поле White —белые
Black — черные Move — ход Check — шах Mate — мат
Checkmate — шах и мат To resign — сдаться Draw — ничья
Рокировка — castling Time pressure (time trouble) — цейтнот
Opening — дебют Endgame — эндшпиль.*/