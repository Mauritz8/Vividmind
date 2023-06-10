#ifndef PIECE_H
#define PIECE_H

enum Piece_type {PAWN, KNIGHT, BISHOP, ROOK, QUEEN, KING};
enum Color {WHITE, BLACK};

struct Piece {
    enum Piece_type piece_type;
    enum Color color;
};

char get_char_representation(struct Piece* piece);

#endif
