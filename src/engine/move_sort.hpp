#pragma once

#include "board/board.hpp"
#include "move.hpp"
#include <vector>

void sort_moves(std::vector<Move> &moves,
                const std::optional<Move> &best_move_prev_depth,
                const Board &board);
