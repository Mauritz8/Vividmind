#ifndef QUEEN_H
#define QUEEN_H

#include <vector>
#include "../move.h"

std::vector<Move> get_queen_psuedo_legal_moves(const Square& start, Board& board);

#endif
