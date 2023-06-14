#ifndef MOVE_H
#define MOVE_H

#include <stdbool.h>

#include "square.h"
#include "board.h"

typedef struct {
    Square* start_square;
    Square* end_square;
} Move;

typedef struct {
    Move* moves;
    int length;
} MoveArray;

void make_move(Move* move, Board* board);
bool is_checkmated(Color color, Board* board);
bool is_legal_move(Move* move, Board* board);

#endif
