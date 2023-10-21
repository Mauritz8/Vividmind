#ifndef PAWN_H
#define PAWN_H

#include <vector>
#include "../move.h"


std::vector<Move> get_pawn_psuedo_legal_moves(Piece pawn, Board& board);
std::vector<Move> get_pawn_captures(Piece pawn, const Board& board);
bool is_valid_en_passant(const Move& pawn_capture, const Board& board);
bool is_promotion_move(const Move& pawn_move);

#endif
