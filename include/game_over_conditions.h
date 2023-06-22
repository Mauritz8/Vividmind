#ifndef GAME_OVER_CONDITIONS_H
#define GAME_OVER_CONDITIONS_H

#include "board.h"
#include "move.h"

bool is_checkmate(Board* board, const MoveArray* move_history);
bool is_draw(Board* board, const MoveArray* move_history);

#endif
