#include <cctype>
#include <optional>

#include "piece.h"


Piece::Piece(Piece_type piece_type, Color color) {
    set_piece_type(piece_type);
    set_color(color);
}

Piece_type Piece::get_piece_type() const {
    return piece_type;
}

void Piece::set_piece_type(Piece_type piece_type) {
    this->piece_type = piece_type;
}

Color Piece::get_color() const {
    return color;
}

void Piece::set_color(Color color) {
    this->color = color;
}

char get_char_representation(Piece_type piece_type) {
    switch (piece_type) {
        case PAWN:
            return 'p';
        case KNIGHT:
            return 'N';
        case BISHOP:
            return 'B';
        case ROOK:
            return 'R';
        case QUEEN:
            return 'Q';
        case KING:
            return 'K';
    }
}

std::optional<Piece_type> get_piece_type(char char_representation) {
    switch (tolower(char_representation)) {
        case 'p': return PAWN;
        case 'n': return KNIGHT;
        case 'b': return BISHOP;
        case 'r': return ROOK;
        case 'q': return QUEEN;
        case 'k': return KING;
        default: return {};
    }
}

std::optional<Piece_type> get_promotion_piece_type(char char_representation_lowercase) {
    if (char_representation_lowercase == 'n') return KNIGHT;
    if (char_representation_lowercase == 'b') return BISHOP;
    if (char_representation_lowercase == 'r') return ROOK;
    if (char_representation_lowercase == 'q') return QUEEN;
    return {};
}
