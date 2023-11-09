#ifndef PIECE_HPP
#define PIECE_HPP

#define KING_VALUE 100000
#define QUEEN_VALUE 900
#define ROOK_VALUE 500
#define BISHOP_VALUE 300
#define KNIGHT_VALUE 300
#define PAWN_VALUE 100

#include "defs.hpp"


class Piece {
    public:
        PieceType piece_type;
        Color color;
        int pos;

        Piece(PieceType piece_type, Color color, int pos);

        bool operator==(Piece piece) const;
        bool operator!=(Piece piece) const;

        char get_char_representation() const;
        int get_value() const;
};

char get_char_representation(PieceType piece_type);
PieceType get_piece_type(char char_representation);

#endif
