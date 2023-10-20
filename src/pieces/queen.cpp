#include "piece.h"
#include "move.h"
#include "pieces/queen.h"
#include <vector>



std::vector<Move> get_queen_psuedo_legal_moves(const Square& start, Board& board) {
    std::vector<Move> moves;

    std::vector<Move> up = get_psuedo_legal_moves_direction(start, 0, 1, board);
    std::vector<Move> up_right = get_psuedo_legal_moves_direction(start, 1, 1, board);
    std::vector<Move> right = get_psuedo_legal_moves_direction(start, 1, 0, board);
    std::vector<Move> down_right = get_psuedo_legal_moves_direction(start, 1, -1, board);
    std::vector<Move> down = get_psuedo_legal_moves_direction(start, 0, -1, board);
    std::vector<Move> down_left = get_psuedo_legal_moves_direction(start, -1, -1, board);
    std::vector<Move> left = get_psuedo_legal_moves_direction(start, -1, 0, board);
    std::vector<Move> up_left = get_psuedo_legal_moves_direction(start, -1, 1, board);

    moves.insert(std::end(moves), std::begin(up), std::end(up));
    moves.insert(std::end(moves), std::begin(up_right), std::end(up_right));
    moves.insert(std::end(moves), std::begin(right), std::end(right));
    moves.insert(std::end(moves), std::begin(down_right), std::end(down_right));
    moves.insert(std::end(moves), std::begin(down), std::end(down));
    moves.insert(std::end(moves), std::begin(down_left), std::end(down_left));
    moves.insert(std::end(moves), std::begin(left), std::end(left));
    moves.insert(std::end(moves), std::begin(up_left), std::end(up_left));


    return moves;
}
