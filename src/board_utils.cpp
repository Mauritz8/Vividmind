#include <cstdlib>

#include "board.h"
#include "board_utils.h"

bool is_same_line(const Square& square1, const Square& square2) {
    return square1.get_x() == square2.get_x() || square1.get_y() == square2.get_y();
}

bool is_same_diagonal(const Square& square1, const Square& square2) {
    return abs(square1.get_x() - square2.get_x()) == abs(square1.get_y() - square2.get_y());
}

bool is_clear_line(const Square& square1, const Square& square2, const Board& board) {
    int x_direction = 0;
    int y_direction = 0;

    if (square1.get_x() != square2.get_x()) {
        x_direction = (square2.get_x() - square1.get_x()) > 0 ? 1 : -1;
    } else if (square1.get_y() != square2.get_y()) {
        y_direction = (square2.get_y() - square1.get_y()) > 0 ? 1 : -1;
    }

    int x = square1.get_x() + x_direction;
    int y = square1.get_y() + y_direction;
    while (x != square2.get_x() || y != square2.get_y()) {
        if (board.get_square(x, y).get_piece()) {
            return false;
        }
        x += x_direction;
        y += y_direction;
    }
    return true;
}

bool is_clear_diagonal(const Square& square1, const Square& square2, const Board& board) {
    if (square1 == square2) {
        return true;
    }

    const int x_direction = (square2.get_x() - square1.get_x()) > 0 ? 1 : -1;
    const int y_direction = (square2.get_y() - square1.get_y()) > 0 ? 1 : -1;

    int x = square1.get_x() + x_direction;
    int y = square1.get_y() + y_direction;
    while (x != square2.get_x() && y != square2.get_y()) {
        if (board.get_square(x, y).get_piece()) {
            return false;
        }
        x += x_direction;
        y += y_direction;
    }
    return true;
}
