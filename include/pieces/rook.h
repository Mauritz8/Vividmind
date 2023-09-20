#ifndef ROOK_H
#define ROOK_H

#include <vector>
#include "../move.h"

class Rook : public Piece {
    public:
        Rook(Color color) : Piece(ROOK, color) {};

    protected:
        std::vector<Move> get_psuedo_legal_moves(const Board& board) const override;
};

#endif
