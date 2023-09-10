#ifndef GAME_OVER_CONDITIONS_H
#define GAME_OVER_CONDITIONS_H

#include "board.h"
#include "move.h"
#include <vector>

bool is_checkmate(const Board& board, const std::vector<Move> move_history);
bool is_draw(const Board& board, const std::vector<Move> move_history);

#endif
