#ifndef BOARD_UTILS_H
#define BOARD_UTILS_H

#include "square.h"

bool is_outside_board(int x, int y);
bool is_same_line(const Square& square1, const Square& square2);
bool is_same_diagonal(const Square& square1, const Square& square2);
bool is_clear_line(const Square& square1, const Square& square2, const Board& board);
bool is_clear_diagonal(const Square& square1, const Square& square2, const Board& board);

#endif
