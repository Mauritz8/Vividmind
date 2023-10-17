#ifndef BISHOP_H
#define BISHOP_H

#include <vector>
#include "../move.h"

class Bishop : public Piece {
    public:
        Bishop(Color color, int x, int y) : Piece(color, x, y) {};
        std::unique_ptr<Piece> clone() const override;
        char get_char_representation() const override;
        std::vector<std::vector<int>> get_piece_square_table() const override;

    protected:
        std::vector<Move> get_psuedo_legal_moves(const Board& board, const std::vector<Move>& move_history) const override;
};

#endif
