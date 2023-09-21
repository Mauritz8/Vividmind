#ifndef BISHOP_H
#define BISHOP_H

#include <vector>
#include "../move.h"

class Bishop : public Piece {
    public:
        Bishop(Color color, int x, int y) : Piece(color, x, y) {};

    protected:
        std::vector<Move> get_psuedo_legal_moves(const Board& board) const override;
};

#endif
