#ifndef KNIGHT_H
#define KNIGHT_H

#include <vector>
#include "../move.h"

std::vector<Move> get_knight_psuedo_legal_moves(const Square& start, Board& board);

#endif
