#include "move.hpp"


Move::Move(int start, int end) {
    this->start = start;
    this->end = end;
    this->is_castling_move = false;
    this->is_promotion = false;
    this->is_en_passant = false;
    this->is_pawn_two_squares_forward = false;
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

