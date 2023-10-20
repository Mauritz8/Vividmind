#ifndef GAME_OVER_CONDITIONS_H
#define GAME_OVER_CONDITIONS_H

#include "board.h"
#include "move.h"
#include <vector>

bool is_checkmate(Board& board);
bool is_draw(Board& board);

#endif
