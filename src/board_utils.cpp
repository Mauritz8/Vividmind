#include <cstdlib>

#include "board.h"
#include "board_utils.h"
#include "move.h"


bool is_outside_board(int x, int y) {
    return x > 7 || x < 0 || y > 7 || y < 0;
}

bool is_same_line(Pos pos1, Pos pos2) {
    return pos1.x == pos2.x || pos1.y == pos2.y;
}

bool is_same_diagonal(Pos pos1, Pos pos2) {
    return abs(pos1.x - pos2.x) == abs(pos1.y - pos2.y);
}

bool is_clear_line(Pos pos1, Pos pos2, const Board& board) {
    int x_direction = 0;
    int y_direction = 0;

    if (pos1.x != pos2.x) {
        x_direction = (pos2.x - pos1.x) > 0 ? 1 : -1;
    } else if (pos1.y != pos2.y) {
        y_direction = (pos2.y - pos1.y) > 0 ? 1 : -1;
    }

    int x = pos1.x + x_direction;
    int y = pos1.y + y_direction;
    while (x != pos2.x || y != pos2.y) {
        if (board.get_square(x, y).get_piece()) {
            return false;
        }
        x += x_direction;
        y += y_direction;
    }
    return true;
}

bool is_clear_diagonal(Pos pos1, Pos pos2, const Board& board) {
    if (pos1.x == pos2.x && pos1.y == pos2.y) {
        return true;
    }

    const int x_direction = (pos2.x - pos1.x) > 0 ? 1 : -1;
    const int y_direction = (pos2.y - pos1.y) > 0 ? 1 : -1;

    int x = pos1.x + x_direction;
    int y = pos1.y + y_direction;
    while (x != pos2.x && y != pos2.y) {
        if (board.get_square(x, y).get_piece()) {
            return false;
        }
        x += x_direction;
        y += y_direction;
    }
    return true;
}
