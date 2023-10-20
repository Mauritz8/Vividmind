#include "move.h"
#include "pieces/rook.h"
#include <iterator>
#include <vector>


std::unique_ptr<Piece> Rook::clone() const {
    return std::make_unique<Rook>(*this);
}

char Rook::get_char_representation() const {
    return 'R';
}

std::vector<std::vector<int>> Rook::get_piece_square_table() const {
    return {
        {  0,  0,  0,  0,  0,  0,  0,  0},
        {  5, 10, 10, 10, 10, 10, 10,  5},
        { -5,  0,  0,  0,  0,  0,  0, -5},
        { -5,  0,  0,  0,  0,  0,  0, -5},
        { -5,  0,  0,  0,  0,  0,  0, -5},
        { -5,  0,  0,  0,  0,  0,  0, -5},
        { -5,  0,  0,  0,  0,  0,  0, -5},
        {  0,  0,  0,  5,  5,  0,  0,  0},
    };
}

int Rook::get_value() const {
    return 5;
}

std::vector<Move> Rook::get_psuedo_legal_moves(Board& board) const {
    std::vector<Move> moves;
    const Square& start = board.get_square(this->get_x(), this->get_y());

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
