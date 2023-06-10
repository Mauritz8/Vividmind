#ifndef SQUARE_H
#define SQUARE_H

#include "piece.h"

struct Square {
    int x;
    int y;
    struct Piece* piece;
};

#endif
