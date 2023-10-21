#ifndef BOARD_UTILS_H
#define BOARD_UTILS_H

#include "move.h"
#include "square.h"


bool is_outside_board(int x, int y);
bool is_occupied_by_color(Pos pos, Color color, const Board& board);
Color get_opposite_color(Color color);
bool is_same_line(Pos pos1, Pos pos2);
bool is_same_diagonal(Pos pos1, Pos pos2);
bool is_clear_line(Pos pos1, Pos pos2, const Board& board);
bool is_clear_diagonal(Pos pos1, Pos pos2, const Board& board);

#endif
