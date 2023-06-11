#ifndef MOVE_H
#define MOVE_H

#include <stdbool.h>

#include "square.h"
#include "board.h"

typedef struct {
    Square* start_square;
    Square* end_square;
} Move;

void make_move(Move* move);
bool is_valid_move(Move* move, Board* board);

#endif
