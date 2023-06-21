#ifndef GAME_OVER_CONDITIONS_H
#define GAME_OVER_CONDITIONS_H

#include "board.h"
#include "move.h"

bool is_game_over(const Color player_to_move, Board* board, const MoveArray* move_history);

#endif
