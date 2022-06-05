#include "Board.h"

MapBoard GenerationBoard(Player * first_player, Player * second_player) {
    const short int size_board = 8;
    MapBoard board;
    Piece piece;
    for (short int i = 0; i < size_board / 2; i++) {
        for (short int j = 0; j < size_board; j++) {
            Pos pos_1(j, i);
            Pos pos_2(j, size_board - i - 1);
            if (i == 0) {
                if (j == 0 || j == size_board - 1) {
                    piece = rook;
                } else if (j == 1 || j == size_board - 2) {
                    piece = knight;
                } else if (j == 2 || j == size_board - 3) {
                    piece = bishop;
                } else if (j == 3) {
                    piece = queen;
                } else {
                    piece = king;
                }
            } else if (i == 1) {
                piece = pawn;
            } else {
                board[pos_1] = nullptr;
                board[pos_2] = nullptr;
                continue;
            }
            if (piece == king) {
                board[pos_1] = new King(pos_1, first_player->GetColor());
                board[pos_2] = new King(pos_2, second_player->GetColor());
            } else if (piece == pawn) {
                board[pos_1] = new Pawn(pos_1, first_player->GetColor(), Orientation::first_player);
                board[pos_2] = new Pawn(pos_2, second_player->GetColor(), Orientation::second_player);
            } else {
                board[pos_1] = new ChessPiece(pos_1, piece, white);
                board[pos_2] = new ChessPiece(pos_2, piece, black);
            }
            first_player->AddPiece(board.at(pos_1));
            second_player->AddPiece(board.at(pos_2));
        }
    }
    return board;
}

Board::Board(Player * first_player, Player * second_player) {
    this->move_player = first_player;
    this->sleep_player = second_player;
    this->board = GenerationBoard(first_player, second_player);
    CheckBoard();
}

Board::~Board() {
    delete move_player;
    delete sleep_player;
    move_player = nullptr;
    sleep_player = nullptr;
    for (auto [key, value] : board) {
        delete value;
        value = nullptr;
    }
}

void Board::CheckBoard() {
     // костыль
    for (auto piece : move_player->GetPieces()) {
        MapMove moves;
        Pos pos = piece->GetPosition();
        pos.first -= 1;
        pos.second -= 1;
        for (int j = 0; j <= 2; j++) {
            for (int i = 0; i <= 2; i++) {
                Pos a = Pos(pos.first + i, pos.second + j);
                if (!(a == piece->GetPosition())) {
                    if (CheckCell(a) == empty) {
                        moves[a] = NULL_CELL;
                    } else if (CheckCell(a) == sleep_piece) {
                        moves[a] = board.at(a)->GetPosition();
                    }
                }
            }
        }
        piece->EditMove(moves);
    }



    // 1) пат
    // 2) мат
    // 3) все фигуры узнают, куда они могут сходить
    // 4) ничья
    // Особые: пешка дошла до края, рокировка


}

StatusCell Board::CheckCell(Pos pos) {
    if (!FindBoard(pos)) {
        return not_exist;
    } else if (move_player->FindPiece(board.at(pos))) {
        return move_piece;
    } else if (sleep_player->FindPiece(board.at(pos))) {
        return sleep_piece;
    } else {
        return empty;
    }
}

Error Board::Move(Pos from, Pos in) {
    if (move_player->FindPiece(AtPiece(from))) {
//        if (FindBoard(in)) { // просто чтобы подвигать фигуры
//            std::swap(board.at(from), board.at(in));
//            board.at(in)->EditPosition(in);
//            if (board.at(from) != nullptr)
//                board.at(from)->EditPosition(from);
//            EditMovePlayer();
//            CheckBoard();
//            return done;
//        }


        if (AtPiece(from)->FindMove(in)) {
            if (CheckCell(AtPiece(from)->GetMove().at(in)) == sleep_piece) {
                sleep_player->PopPiece(board.at(in));
                delete board.at(in);
                board.at(in) = nullptr;
            }
            std::swap(board.at(from), board.at(in));
            board.at(in)->EditPosition(in);
            EditMovePlayer();
            CheckBoard();
            return done;
        } else {
            return in_error;
        }
    } else {
        return from_error;
    }
}

ChessPiece * Board::AtPiece(const Pos key) {
    if (FindBoard(key)) {
        return board.at(key);
    }
    return nullptr;
}
