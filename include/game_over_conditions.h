#ifndef GAME_OVER_CONDITIONS_H
#define GAME_OVER_CONDITIONS_H

#include "board.h"
#include "move.h"
#include <vector>

bool is_checkmate(Board& board, std::vector<Move>& legal_moves);
bool is_draw(Board& board, std::vector<Move>& legal_moves);

#endif
