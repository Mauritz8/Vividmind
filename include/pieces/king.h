#ifndef KING_H
#define KING_H

#include <vector>
#include "../move.h"

class King : public Piece {
    public:
        King(Color color, int x, int y) : Piece(color, x, y) {};
        std::unique_ptr<Piece> clone() const override;
        char get_char_representation() const override;
        std::vector<Move> get_threatened_moves(const Board& board) const;
        bool is_valid_castling(const Move& move, const Board& board) const;
        std::vector<std::vector<int>> get_piece_square_table() const override;
        int get_value() const override;

    protected:
        std::vector<Move> get_psuedo_legal_moves(const Board& board) const override;

    private:
        std::vector<Move> get_castling_moves(const Board& board) const;
        bool passes_through_check_when_castling(const Move& castling_move, const Board& board) const;
        std::vector<Move> get_potential_castling_moves(const Board& board) const;
};

#endif
