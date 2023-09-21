#ifndef PAWN_H
#define PAWN_H

#include <vector>
#include "../move.h"

class Pawn : public Piece {
    public:
        Pawn(Color color, int x, int y) : Piece(color, x, y) {};
        char get_char_representation() const override;

    protected:
        std::vector<Move> get_psuedo_legal_moves(const Board& board) const override;
};

#endif
