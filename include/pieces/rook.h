#ifndef ROOK_H
#define ROOK_H

#include <vector>
#include "../move.h"

class Rook : public Piece {
    public:
        Rook(Color color, int x, int y) : Piece(color, x, y) {};
        std::unique_ptr<Piece> clone() const override;
        char get_char_representation() const override;

    protected:
        std::vector<Move> get_psuedo_legal_moves(const Board& board, const std::vector<Move>& move_history) const override;
};

#endif
