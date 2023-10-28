#include "move.hpp"

#include <iostream>

#include "board_helper.hpp"
#include "board_utils.hpp"
#include "move_validator.hpp"


Move::Move(int start_x, int start_y, int end_x, int end_y) {
    this->start = {start_x, start_y};
    this->end = {end_x, end_y};
    this->is_castling_move = false;
    this->is_promotion = false;
    this->is_en_passant = false;
    this->is_pawn_two_squares_forward = false;
}

Move::Move(Pos start, Pos end) {
    this->start = start;
    this->end = end;
    this->is_castling_move = false;
    this->is_promotion = false;
    this->is_en_passant = false;
    this->is_pawn_two_squares_forward = false;
}

Move::Move(const Square& start, const Square& end) {
    this->start = Pos{start.x, start.y};
    this->end = Pos{end.x, end.y};
    this->is_castling_move = false;
    this->is_promotion = false;
    this->is_en_passant = false;
    this->is_pawn_two_squares_forward = false;
}

Move Move::get_from_uci_notation(const std::string& uci_notation, Board& board) {
    const int start_x = uci_notation[0] - 'a';
    const int start_y = 8 - (uci_notation[1] - '0');
    const int end_x = uci_notation[2] - 'a';
    const int end_y = 8 - (uci_notation[3] - '0');
    Move move = Move(start_x, start_y, end_x, end_y);

    const Square& start = board.get_square(start_x, start_y);
    const Square& end = board.get_square(end_x, end_y);

    BoardHelper board_helper = BoardHelper(board);
    MoveValidator move_validator = MoveValidator(board, board_helper);
    if (start.piece->piece_type == KING && move_validator.is_valid_castling_move(move)) {
        move.is_castling_move = true;
    } else if (start.piece->piece_type == PAWN) {
        if (move_validator.is_valid_en_passant(move)) {
            move.is_en_passant = true;
        } else if (move_validator.is_promotion_move(move)) {
            move.is_promotion = true;
            const char promotion_piece = uci_notation[4];
            move.promotion_piece = get_piece_type(promotion_piece);
        } else if (abs(move.end.y - move.start.y) == 2) {
            move.is_pawn_two_squares_forward = true;
        }
    }
    return move;
}

Move::Move(const Move& move) {
    this->start = move.start;
    this->end = move.end;
    this->is_castling_move = move.is_castling_move;
    this->is_promotion = move.is_promotion;
    this->promotion_piece = move.promotion_piece;
    this->is_en_passant = move.is_en_passant;
    this->is_pawn_two_squares_forward = move.is_pawn_two_squares_forward;
}


Move Move::operator=(const Move& move) {
    this->start = move.start;
    this->end = move.end;
    this->is_castling_move = move.is_castling_move;
    this->is_promotion = move.is_promotion;
    this->promotion_piece = move.promotion_piece;
    this->is_en_passant = move.is_en_passant;
    this->is_pawn_two_squares_forward = move.is_pawn_two_squares_forward;
    return *this;
}

std::string Move::to_uci_notation() const {
    const std::string files = "abcdefgh";
    const std::string ranks = "87654321";

    std::string uci_notation;
    uci_notation += files[this->start.x];
    uci_notation += ranks[this->start.y];
    uci_notation += files[this->end.x];
    uci_notation += ranks[this->end.y];

    if (this->promotion_piece) {
        uci_notation += tolower(get_char_representation(this->promotion_piece.value()));
    }
    return uci_notation;
}

