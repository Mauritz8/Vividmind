#include "piece.hpp"

#include <cctype>
#include <stdexcept>

#include "engine/psqt.hpp"


Piece::Piece(PieceType piece_type, Color color, int pos) {
    this->piece_type = piece_type;
    this->color = color;
    this->pos = pos;
}

bool Piece::operator==(Piece piece) const {
    return piece.pos == pos
        && piece.color == color 
        && piece.piece_type == piece_type;
}

bool Piece::operator!=(Piece piece) const {
    return !(*this == piece);
}

char Piece::get_char_representation() const {
    return ::get_char_representation(piece_type);
}

int Piece::get_value() const {
    switch (piece_type) {
        case KING: return KING_VALUE;
        case QUEEN: return QUEEN_VALUE;
        case ROOK: return ROOK_VALUE;
        case BISHOP: return BISHOP_VALUE;
        case KNIGHT: return KNIGHT_VALUE;
        case PAWN: return PAWN_VALUE;
    }
}

int Piece::get_psqt_score() const {
    int square = color == WHITE ? pos : 56 - pos + pos % 8;
    switch (piece_type) {
        case KING: return KING_PSQT[square];
        case QUEEN: return QUEEN_PSQT[square];
        case ROOK: return ROOK_PSQT[square];
        case BISHOP: return BISHOP_PSQT[square];
        case KNIGHT: return KNIGHT_PSQT[square];
        case PAWN: return PAWN_PSQT[square];
    }
}


char get_char_representation(PieceType piece_type) {
    switch (piece_type) {
        case PAWN: return 'p';
        case KNIGHT: return 'N';
        case BISHOP: return 'B';
        case ROOK: return 'R';
        case QUEEN: return 'Q';
        case KING: return 'K';
    }
    throw std::invalid_argument("invalid piece type");
}

PieceType get_piece_type(char char_representation) {
    switch (tolower(char_representation)) {
        case 'p': return PAWN;
        case 'n': return KNIGHT;
        case 'b': return BISHOP;
        case 'r': return ROOK;
        case 'q': return QUEEN;
        case 'k': return KING;
    }
    throw std::invalid_argument("invalid piece type");
}
