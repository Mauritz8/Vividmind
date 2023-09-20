#ifndef BISHOP_H
#define BISHOP_H

#include <vector>
#include "../move.h"

class Bishop : public Piece {
    public:
        Bishop(Color color) : Piece(BISHOP, color) {};

    protected:
        std::vector<Move> get_psuedo_legal_moves(const Board& board) const override;
};

#endif
