#ifndef PIECE_H
#define PIECE_H

typedef enum {PAWN, KNIGHT, BISHOP, ROOK, QUEEN, KING} Piece_type;
typedef enum {WHITE, BLACK} Color;

typedef struct {
    Piece_type piece_type;
    Color color;
} Piece;

typedef struct {
    Piece* pieces;
    int capacity;
    int length;
} PieceArray;

void piece_array_push(PieceArray* piece_array, const Piece* piece);
char get_char_representation(const Piece_type piece_type);
Piece_type get_promotion_piece_type(const char char_representation_lowercase);

#endif
