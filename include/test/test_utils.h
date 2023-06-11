#ifndef TEST_UTILS_H
#define TEST_UTILS_H

#include "board.h"
#include "piece.h"

void setup_empty_board(struct Board* board);
void place_piece_at(struct Piece* piece, struct Board* board, int x, int y);

#endif
