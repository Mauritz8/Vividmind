#include "move.h"
#include "piece.h"
#include "pieces/bishop.h"
#include <iterator>
#include <vector>


char Bishop::get_char_representation() const {
    return 'B';
}

std::vector<Move> Bishop::get_psuedo_legal_moves(const Board& board) const  {
    std::vector<Move> moves;
    const Square& start = board.get_square(this->get_x(), this->get_y());

    std::vector<Move> up_right = get_psuedo_legal_moves_direction(start, 1, 1, board);
    std::vector<Move> down_right = get_psuedo_legal_moves_direction(start, 1, -1, board);
    std::vector<Move> down_left = get_psuedo_legal_moves_direction(start, -1, -1, board);
    std::vector<Move> up_left = get_psuedo_legal_moves_direction(start, -1, 1, board);

    moves.insert(std::end(moves), std::begin(up_right), std::end(up_right));
    moves.insert(std::end(moves), std::begin(down_right), std::end(down_right));
    moves.insert(std::end(moves), std::begin(down_left), std::end(down_left));
    moves.insert(std::end(moves), std::begin(up_left), std::end(up_left));

    return moves;
}
