#ifndef KNIGHT_H
#define KNIGHT_H

#include <vector>
#include "../move.h"

class Knight : public Piece {
    public:
        Knight(Color color) : Piece(KNIGHT, color) {};

    protected:
        std::vector<Move> get_psuedo_legal_moves(const Board& board) const override;
};

#endif
