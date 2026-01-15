#pragma once

#include "board/board.hpp"
#include "move.hpp"
#include <unordered_set>
#include <vector>

void sort_moves(std::vector<Move> &moves,
                const std::optional<Move> &best_move_prev_depth,
                std::unordered_set<Move, Move::HashFunction> &killer_moves,
                const Board &board);
