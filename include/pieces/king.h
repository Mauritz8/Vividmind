#ifndef KING_H
#define KING_H

#include <vector>
#include "../move.h"

std::vector<Move> get_king_psuedo_legal_moves(const Square& start, Board& board);
std::vector<Move> get_king_threatened_moves(const Square& start, const Board& board);
bool is_valid_castling(const Move& move, Board& board);

#endif
