#ifndef PIECE_H
#define PIECE_H

typedef enum {PAWN, KNIGHT, BISHOP, ROOK, QUEEN, KING} Piece_type;
typedef enum {WHITE, BLACK} Color;

typedef struct {
    Piece_type piece_type;
    Color color;
} Piece;

char get_char_representation(Piece* piece);

#endif
