#ifndef QUEEN_H
#define QUEEN_H

#include <vector>
#include "../move.h"

class Queen : public Piece {
    public:
        Queen(Color color, int x, int y) : Piece(color, x, y) {};
        std::unique_ptr<Piece> clone() const override;
        char get_char_representation() const override;

    protected:
        std::vector<Move> get_psuedo_legal_moves(const Board& board, const std::vector<Move>& move_history) const override;
};

#endif
