#include "move.hpp"


Move::Move(int start, int end) {
    this->start = start;
    this->end = end;
    this->move_type = NORMAL;
}

Move::Move(const Move& move) {
    this->start = move.start;
    this->end = move.end;
    this->move_type = move.move_type;
    this->promotion_piece = move.promotion_piece;
}

Move Move::operator=(const Move& move) {
    this->start = move.start;
    this->end = move.end;
    this->move_type = move.move_type;
    this->promotion_piece = move.promotion_piece;
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

