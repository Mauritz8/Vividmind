#ifndef PIECE_H
#define PIECE_H

enum Piece_type {PAWN, KNIGHT, BISHOP, ROOK, QUEEN, KING};
enum Color {WHITE, BLACK};

typedef struct Piece {
    enum Piece_type piece_type;
    enum Color color;
} Piece;

char get_char_representation(Piece* piece);

#endif
