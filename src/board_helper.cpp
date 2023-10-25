#include "board_helper.h"

#include <stdexcept>

#include "board_utils.h"


BoardHelper::BoardHelper(const Board& board)
    : board(board)
{}


bool BoardHelper::is_occupied_by_color(Pos pos, Color color) const {
    const Square& end = board.get_square(pos.x, pos.y);
    return end.piece && end.piece->color == color;
}

bool BoardHelper::is_clear_line(Pos pos1, Pos pos2) const {
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
        if (board.get_square(x, y).piece) {
            return false;
        }
        x += x_direction;
        y += y_direction;
    }
    return true;
}

bool BoardHelper::is_clear_diagonal(Pos pos1, Pos pos2) const {
    if (pos1 == pos2) {
        return true;
    }

    const int x_direction = (pos2.x - pos1.x) > 0 ? 1 : -1;
    const int y_direction = (pos2.y - pos1.y) > 0 ? 1 : -1;

    int x = pos1.x + x_direction;
    int y = pos1.y + y_direction;
    while (x != pos2.x && y != pos2.y) {
        if (board.get_square(x, y).piece) {
            return false;
        }
        x += x_direction;
        y += y_direction;
    }
    return true;
}

Pos BoardHelper::get_king_square(Color color) const {
    for (Piece piece : board.game_state.pieces[color]) {
        if (piece.piece_type == KING) {
            return piece.pos;
        }
    }
    std::string color_str = color == WHITE ? "white" : "black";
    throw std::invalid_argument("No " + color_str + " king on the board");
}
