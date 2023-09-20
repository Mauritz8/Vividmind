#ifndef KING_H
#define KING_H

#include <vector>
#include "../move.h"

class King : public Piece {
    public:
        King(Color color) : Piece(KING, color) {};

    protected:
        std::vector<Move> get_psuedo_legal_moves(const Board& board) const override;
};

#endif
