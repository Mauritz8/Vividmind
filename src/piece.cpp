#include "piece.h"

#include <cctype>
#include <stdexcept>

#include "engine/psqt.h"


Piece::Piece(PieceType piece_type, Color color, Pos pos) {
    this->piece_type = piece_type;
    this->color = color;
    this->pos.x = pos.x;
    this->pos.y = pos.y;
}

bool Piece::operator==(Piece piece) const {
    return piece.pos.x == pos.x
        && piece.pos.y == pos.y 
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
    int y = color == WHITE ? pos.y : 7 - pos.y;
    switch (piece_type) {
        case KING: return KING_PSQT[y][pos.x];
        case QUEEN: return QUEEN_PSQT[y][pos.x];
        case ROOK: return ROOK_PSQT[y][pos.x];
        case BISHOP: return BISHOP_PSQT[y][pos.x];
        case KNIGHT: return KNIGHT_PSQT[y][pos.x];
        case PAWN: return PAWN_PSQT[y][pos.x];
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
