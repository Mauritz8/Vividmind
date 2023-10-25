#include "board_utils.h"

#include <cmath>


bool is_outside_board(int x, int y) {
    return x > 7 || x < 0 || y > 7 || y < 0;
}

Color get_opposite_color(Color color) {
    return color == WHITE ? BLACK : WHITE;
}

bool is_same_line(Pos pos1, Pos pos2) {
    return pos1.x == pos2.x || pos1.y == pos2.y;
}

bool is_same_diagonal(Pos pos1, Pos pos2) {
    return abs(pos1.x - pos2.x) == abs(pos1.y - pos2.y);
}
