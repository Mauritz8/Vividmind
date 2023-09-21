#ifndef QUEEN_H
#define QUEEN_H

#include <vector>
#include "../move.h"

class Queen : public Piece {
    public:
        Queen(Color color, int x, int y) : Piece(color, x, y) {};

    protected:
        std::vector<Move> get_psuedo_legal_moves(const Board& board) const override;
};

#endif
