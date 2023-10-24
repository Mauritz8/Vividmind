#ifndef KING_H
#define KING_H

#include <vector>
#include "../move.h"


std::vector<Move> get_king_threatened_moves(Piece king, const Board& board, bool only_captures);
std::vector<Move> get_castling_moves(Board& board);
bool is_valid_castling(const Move& move, Board& board);

#endif
