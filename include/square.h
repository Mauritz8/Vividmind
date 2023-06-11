#ifndef SQUARE_H
#define SQUARE_H

#include "piece.h"

typedef struct {
    int x;
    int y;
    Piece* piece;
} Square;

#endif
