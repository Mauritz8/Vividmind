#include <cctype>

#include "piece.h"

char Piece::get_char_representation() {
    switch (get_piece_type()) {
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

Piece_type Piece::get_piece_type(const char char_representation) {
    switch (tolower(char_representation)) {
        case 'p': return PAWN;
        case 'n': return KNIGHT;
        case 'b': return BISHOP;
        case 'r': return ROOK;
        case 'q': return QUEEN;
        case 'k': return KING;
        default: return -1;
    }
}

Piece_type Piece::get_promotion_piece_type(const char char_representation_lowercase) {
    if (char_representation_lowercase == 'n') return KNIGHT;
    if (char_representation_lowercase == 'b') return BISHOP;
    if (char_representation_lowercase == 'r') return ROOK;
    if (char_representation_lowercase == 'q') return QUEEN;
    return -1;
}
