#ifndef BOARD_UTILS_H
#define BOARD_UTILS_H

#include "piece.hpp"
#include "pos.hpp"


bool is_outside_board(int x, int y);
Color get_opposite_color(Color color);
bool is_same_line(Pos pos1, Pos pos2);
bool is_same_diagonal(Pos pos1, Pos pos2);

#endif
