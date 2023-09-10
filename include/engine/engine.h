#ifndef ENGINE_H
#define ENGINE_H

#include "move.h"
#include "board.h"
#include <vector>

Move get_best_move(int depth, const Board& board, const std::vector<Move> move_history);

#endif
