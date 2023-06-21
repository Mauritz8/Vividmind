#ifndef ENGINE_H
#define ENGINE_H

#include "move.h"
#include "board.h"

Move get_best_move(const Color player_to_move, Board* board, MoveArray* move_history);

#endif
