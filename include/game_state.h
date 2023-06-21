#ifndef GAME_STATE_H
#define GAME_STATE_H

#include "board.h"
#include "move.h"
#include "piece.h"

SquareArray get_all_threatened_squares(const Color color, Board* board);
MoveArray get_all_legal_moves(const Color color, Board* board, const MoveArray* move_history);
bool is_in_check(const Color color, Board* board);
PieceArray get_all_pieces(const Color color, Board* board);

#endif
