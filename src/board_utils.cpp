#include "board_utils.hpp"

#include <cmath>


Color get_opposite_color(Color color) {
    return color == WHITE ? BLACK : WHITE;
}

bool is_same_line(int pos1, int pos2) {
    return abs(pos1 % 8 - pos2 % 8) == 0 || pos1 / 8 == pos2 / 8;
}

bool is_same_diagonal(int pos1, int pos2) {
    return abs(pos1 % 8 - pos2 % 8) == (pos1 - pos2) / 8;
}
