#ifndef QUEEN_H
#define QUEEN_H

#include <vector>
#include "../move.h"

class Queen : public Piece {
    public:
        Queen(Color color, int x, int y) : Piece(color, x, y) {};
        std::unique_ptr<Piece> clone() const override;
        char get_char_representation() const override;
        std::vector<std::vector<int>> get_piece_square_table() const override;
        int get_value() const override;

    protected:
        std::vector<Move> get_psuedo_legal_moves(const Board& board) const override;
};

#endif
