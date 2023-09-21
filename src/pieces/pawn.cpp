#include "game_state.h"
#include "move.h"
#include "piece.h"
#include "pieces/pawn.h"
#include <array>
#include <vector>

std::vector<Move> Pawn::get_psuedo_legal_moves(const Board& board) const {
    std::vector<Move> moves;
    const Square& start = board.get_square(this->get_x(), this->get_y());


    const Square& end1 = board.get_square(start.get_x(), start.get_y() + 1);
    if (!end1.get_piece()) {
        moves.push_back(Move(start, end1));

        const Square& end2 = board.get_square(start.get_x(), start.get_y() + 2);
        if (!end2.get_piece()) {
            moves.push_back(Move(start, end2));
        }
    }

    const Square& end3 = board.get_square(start.get_x() + 1, start.get_y() + 1);
    if (end3.get_piece() && end3.get_piece()->get_color() == this->get_color()) {
        moves.push_back(Move(start, end3));
    }
    const Square& end4 = board.get_square(start.get_x() - 1, start.get_y() + 1);
    if (end3.get_piece() && end3.get_piece()->get_color() == this->get_color()) {
        moves.push_back(Move(start, end4));
    }

    return moves;
}
