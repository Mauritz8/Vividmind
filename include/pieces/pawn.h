#ifndef PAWN_H
#define PAWN_H

#include <vector>
#include "../move.h"

class Pawn : public Piece {
    public:
        Pawn(Color color, int x, int y) : Piece(color, x, y) {};
        std::unique_ptr<Piece> clone() const override;
        char get_char_representation() const override;
        std::vector<Move> get_threatened_moves(const Board& board, const std::vector<Move>& move_history) const override;

    protected:
        std::vector<Move> get_psuedo_legal_moves(const Board& board, const std::vector<Move>& move_history) const override;

    private:
        bool is_valid_en_passant(const Move& move, const Board& board, const std::vector<Move>& move_history) const; 
        bool is_promotion_move(const Move& move) const;
};

#endif
