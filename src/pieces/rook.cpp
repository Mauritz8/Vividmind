#include "move.h"
#include "pieces/rook.h"
#include <iterator>
#include <vector>


std::vector<Move> get_rook_psuedo_legal_moves(const Square& start, Board& board) {
    std::vector<Move> moves;

    std::vector<Move> up = get_psuedo_legal_moves_direction(start, 0, 1, board);
    std::vector<Move> right = get_psuedo_legal_moves_direction(start, 1, 0, board);
    std::vector<Move> down = get_psuedo_legal_moves_direction(start, 0, -1, board);
    std::vector<Move> left = get_psuedo_legal_moves_direction(start, -1, 0, board);

    moves.insert(std::end(moves), std::begin(up), std::end(up));
    moves.insert(std::end(moves), std::begin(right), std::end(right));
    moves.insert(std::end(moves), std::begin(down), std::end(down));
    moves.insert(std::end(moves), std::begin(left), std::end(left));

    return moves;
}
