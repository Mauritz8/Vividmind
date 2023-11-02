#include "move.hpp"

#include <iostream>

#include "board_helper.hpp"
#include "board_utils.hpp"
#include "move_validator.hpp"


Move::Move(int start, int end) {
    this->start = start;
    this->end = end;
    this->is_castling_move = false;
    this->is_promotion = false;
    this->is_en_passant = false;
    this->is_pawn_two_squares_forward = false;
}

Move::Move(const Square& start, const Square& end) {
    this->start = start.pos;
    this->end = end.pos;
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

    const int start_pos = start_x + start_y * 8;
    const int end_pos = end_x + end_y * 8;
    Move move = Move(start_pos, end_pos);

    const Square& start = board.squares[start_pos];
    const Square& end = board.squares[end_pos];

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
        } else if (abs((move.end - move.start) / 8) == 2) {
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

bool Move::operator==(const Move& move) const {
    return move.start == start && move.end == end;
}

std::string Move::to_uci_notation() const {
    const std::string files = "abcdefgh";
    const std::string ranks = "87654321";

    std::string uci_notation;
    uci_notation += files[this->start % 8];
    uci_notation += ranks[this->start / 8];
    uci_notation += files[this->end % 8];
    uci_notation += ranks[this->end / 8];

    if (this->promotion_piece) {
        uci_notation += tolower(get_char_representation(this->promotion_piece.value()));
    }
    return uci_notation;
}

