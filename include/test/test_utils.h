#ifndef TEST_UTILS_H
#define TEST_UTILS_H

#include "board.h"
#include "move.h"
#include "piece.h"

void setup_empty_board(Board* board);
void place_piece_at(Piece* piece, Board* board, int x, int y);
MoveArray create_emtpy_move_history();

#endif
