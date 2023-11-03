#include "board_helper.hpp"

#include <iostream>
#include <optional>
#include <stdexcept>

#include "board_utils.hpp"


BoardHelper::BoardHelper(Board& board)
    : board(board)
{}


bool BoardHelper::is_clear_line(int pos1, int pos2) const {
    int x_diff = pos2 % 8 - pos1 % 8;
    int y_diff = pos2 / 8 - pos1 / 8;
    int x_direction = x_diff > 0 ? 1 : -1;
    int y_direction = y_diff > 0 ? 1 : -1;

    int step = x_direction + y_diff * 8;
    int pos = pos1 + step;
    while (pos != pos2) {
        if (board.squares[pos].piece) {
            return false;
        }
        pos += step;
    }
    return true;
}

bool BoardHelper::is_clear_diagonal(int pos1, int pos2) const {
    if (pos1 == pos2) {
        return true;
    }

    int x_diff = pos2 % 8 - pos1 % 8;
    int y_diff = pos2 / 8 - pos1 / 8;
    const int x_direction = x_diff > 0 ? 1 : -1;
    const int y_direction = y_diff > 0 ? 1 : -1;

    int step = x_direction + y_diff * 8;
    int pos = pos1 + step;
    while (pos != pos2) {
        if (board.squares[pos].piece) {
            return false;
        }
        pos += step;
    }
    return true;
}

int BoardHelper::get_king_square(Color color) const {
    for (Piece piece : board.game_state.pieces[color]) {
        if (piece.piece_type == KING) {
            return piece.pos;
        }
    }
    std::string color_str = color == WHITE ? "white" : "black";
    throw std::invalid_argument("No " + color_str + " king on the board");
}
