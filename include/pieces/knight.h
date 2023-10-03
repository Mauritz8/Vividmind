#ifndef KNIGHT_H
#define KNIGHT_H

#include <vector>
#include "../move.h"

class Knight : public Piece {
    public:
        Knight(Color color, int x, int y) : Piece(color, x, y) {};
        char get_char_representation() const override;

    protected:
        std::vector<Move> get_psuedo_legal_moves(const Board& board, const std::vector<Move>& move_history) const override;
};

#endif
