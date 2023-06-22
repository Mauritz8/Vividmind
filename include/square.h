#ifndef SQUARE_H
#define SQUARE_H

#include "piece.h"

typedef struct {
    int x;
    int y;
    Piece* piece;
} Square;

typedef struct {
    Square** squares;
    int capacity;
    int length;
} SquareArray;

void square_array_push(SquareArray* square_array, Square* square);

#endif
