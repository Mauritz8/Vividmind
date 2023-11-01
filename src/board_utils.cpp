#include "board_utils.hpp"

#include <cmath>

bool is_outside_board(int x, int y) {
    return x < 0 || x > 7 || y < 0 || y > 7;
}

bool is_outside_board(int pos) {
    return pos < 0 || pos > 63;
}

Color get_opposite_color(Color color) {
    return color == WHITE ? BLACK : WHITE;
}

bool is_same_line(int pos1, int pos2) {
    return abs(pos1 % 8 - pos2 % 8) == 0 || pos1 / 8 == pos2 / 8;
}

bool is_same_diagonal(int pos1, int pos2) {
    return abs(pos1 % 8 - pos2 % 8) == (pos1 - pos2) / 8;
}
