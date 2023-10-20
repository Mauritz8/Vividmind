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
    std::vector<Move> moves;
    const Square& start = board.get_square(this->get_x(), this->get_y());

    const std::array<Pos, 8> end_squares = {
        Pos{start.get_x() + 1, start.get_y() + 2},
        Pos{start.get_x() + 1, start.get_y() - 2},
        Pos{start.get_x() - 1, start.get_y() + 2},
        Pos{start.get_x() - 1, start.get_y() - 2},
        Pos{start.get_x() + 2, start.get_y() + 1},
        Pos{start.get_x() + 2, start.get_y() - 1},
        Pos{start.get_x() - 2, start.get_y() + 1},
        Pos{start.get_x() - 2, start.get_y() - 1}
    };
    for (Pos end_pos : end_squares) {
        try {
            const Square& end = board.get_square(
                    end_pos.x,
                    end_pos.y);
            moves.push_back(Move(start, end));
        } catch (const std::invalid_argument& e) {}
    }

    return moves;
}
