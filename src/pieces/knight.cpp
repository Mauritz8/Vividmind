#include "game_state.h"
#include "move.h"
#include "piece.h"
#include "pieces/knight.h"
#include <array>
#include <stdexcept>
#include <utility>
#include <vector>


std::unique_ptr<Piece> Knight::clone() const {
    return std::make_unique<Knight>(*this);
}

char Knight::get_char_representation() const {
    return 'N';
}

std::vector<std::vector<int>> Knight::get_piece_square_table() const {
    return {
        {-50,-40,-30,-30,-30,-30,-40,-50},
        {-40,-20,  0,  0,  0,  0,-20,-40},
        {-30,  0, 10, 15, 15, 10,  0,-30},
        {-30,  5, 15, 20, 20, 15,  5,-30},
        {-30,  0, 15, 20, 20, 15,  0,-30},
        {-30,  5, 10, 15, 15, 10,  5,-30},
        {-40,-20,  0,  5,  5,  0,-20,-40},
        {-50,-40,-30,-30,-30,-30,-40,-50},
    };
}

int Knight::get_value() const {
    return 3;
}

std::vector<Move> Knight::get_psuedo_legal_moves(Board& board) const {
    const Pos start = Pos{this->get_x(), this->get_y()};
    const std::array<Pos, 8> end_squares = {
        Pos{start.x + 1, start.y + 2},
        Pos{start.x + 1, start.y - 2},
        Pos{start.x - 1, start.y + 2},
        Pos{start.x - 1, start.y - 2},
        Pos{start.x + 2, start.y + 1},
        Pos{start.x + 2, start.y - 1},
        Pos{start.x - 2, start.y + 1},
        Pos{start.x - 2, start.y - 1}
    };

    std::vector<Move> moves;
    for (Pos end : end_squares) {
        if (!is_outside_board(end)) {
            moves.push_back(Move(start, end));
        }
    }
    return moves;
}
