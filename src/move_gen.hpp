#ifndef MOVE_GEN_HPP
#define MOVE_GEN_HPP

#include "board.hpp"

enum MoveCategory { ALL, TACTICAL };

namespace movegen {
std::vector<Move> get_pseudo_legal_moves(const Board &board,
                                         MoveCategory move_category);
int perft(Board &board, int depth);
void divide(Board &board, int depth);
bool is_in_check(const Board &board, Color color);
}; // namespace movegen

#endif
