#ifndef TEST_UTILS_H
#define TEST_UTILS_H

#include "board.h"
#include "move.h"
#include "piece.h"

void setup_empty_board(Board* board);
void place_piece_at(const Piece* piece, Board* board, const int x, const int y);
MoveArray create_empty_move_history();

#endif
