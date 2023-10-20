#ifndef BISHOP_H
#define BISHOP_H

#include <vector>
#include "../move.h"

std::vector<Move> get_bishop_psuedo_legal_moves(const Square& start, Board& board);

#endif
