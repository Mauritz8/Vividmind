#ifndef MOVE_H
#define MOVE_H

#include <stdbool.h>

#include "square.h"
#include "board.h"

struct Move {
    struct Square* start_square;
    struct Square* end_square;
};

void make_move(struct Move* move);
bool is_valid_move(struct Move* move, struct Board* board);

#endif
