#ifndef ENGINE_H
#define ENGINE_H

#include "move.h"
#include "board.h"

Move get_best_move(const int depth, const Board* board, const MoveArray* move_history);

#endif
