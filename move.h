#ifndef MOVE_H
#define MOVE_H

#include "square.h"

struct Move {
    struct Square* start_square;
    struct Square* end_square;
};

void make_move(struct Move);

#endif
