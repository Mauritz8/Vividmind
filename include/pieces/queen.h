#ifndef QUEEN_H
#define QUEEN_H

#include <vector>
#include "../move.h"

class Queen : public Piece {
    public:
        Queen(Color color) : Piece(QUEEN, color) {};

    protected:
        std::vector<Move> get_psuedo_legal_moves(const Board& board) const override;
};

#endif
