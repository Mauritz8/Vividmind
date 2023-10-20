#ifndef ROOK_H
#define ROOK_H

#include <vector>
#include "../move.h"

std::vector<Move> get_rook_psuedo_legal_moves(const Square& start, Board& board);

#endif
