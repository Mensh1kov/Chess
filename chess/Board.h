#pragma once
#include "Player.h"

const Pos NULL_CELL(NULL, NULL);
enum StatusCell {move_piece, sleep_piece, empty, not_exist};
enum Error {from_error, in_error, done};
using MapBoard = std::map<Pos, ChessPiece * >;

class Board {
    Player * move_player;
    Player * sleep_player;
    MapBoard board;

    void CheckBoard(); // запускается после сделанного хода и изменяет statys у player
public:
    Board(Player * first_player, Player * second_player);

    ~Board();
    Player & GetMovePlayer() { return * move_player; }
    Player & GetSleepPlayer() { return * sleep_player; }
    const MapBoard & GetBoard() { return board; }
    ChessPiece * AtPiece(const Pos key);
    bool FindBoard(const Pos key) const { return board.find(key) != board.end(); }

    // все фигуры ходящего игрока должны знать, куда они могут сходить
    StatusCell CheckCell(Pos pos);
    Error Move(Pos from, Pos in);

    void EditMovePlayer() { std::swap(move_player, sleep_player); }
};