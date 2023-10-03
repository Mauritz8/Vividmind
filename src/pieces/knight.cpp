#include "game_state.h"
#include "move.h"
#include "piece.h"
#include "pieces/knight.h"
#include <array>
#include <vector>

char Knight::get_char_representation() const {
    return 'N';
}

std::vector<Move> Knight::get_psuedo_legal_moves(const Board& board, const std::vector<Move>& move_history) const {
    std::vector<Move> moves;
    const Square& start = board.get_square(this->get_x(), this->get_y());
    const std::array<Square, 8>& end = {
        board.get_square(start.get_x() + 1, start.get_y() + 2),
        board.get_square(start.get_x() + 1, start.get_y() - 2),
        board.get_square(start.get_x() - 1, start.get_y() + 2),
        board.get_square(start.get_x() - 1, start.get_y() - 2),
        board.get_square(start.get_x() + 2, start.get_y() + 1),
        board.get_square(start.get_x() + 2, start.get_y() - 1),
        board.get_square(start.get_x() - 2, start.get_y() + 1),
        board.get_square(start.get_x() - 2, start.get_y() - 1)
    };
    for (const Square& square : end) {
        moves.push_back(Move(start, square));
    }

    return moves;
}
