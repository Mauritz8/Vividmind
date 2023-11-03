#ifndef BOARD_UTILS_H
#define BOARD_UTILS_H

#include "piece.hpp"


Color get_opposite_color(Color color);
bool is_same_line(int pos1, int pos2);
bool is_same_diagonal(int pos1, int pos2);

#endif
