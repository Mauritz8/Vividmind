#ifndef PAWN_H
#define PAWN_H

#include <vector>
#include "../move.h"

class Pawn : public Piece {
    public:
        Pawn(Color color) : Piece(PAWN, color) {};

    protected:
        std::vector<Move> get_psuedo_legal_moves(const Board& board) const override;
};

#endif
