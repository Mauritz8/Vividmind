#ifndef KING_H
#define KING_H

#include <vector>
#include "../move.h"

class King : public Piece {
    public:
        King(Color color, int x, int y) : Piece(color, x, y) {};
        char get_char_representation() const override;

    protected:
        std::vector<Move> get_psuedo_legal_moves(const Board& board, const std::vector<Move>& move_history) const override;

    private:
        bool is_on_starting_square() const;
        bool is_valid_castling(const Move& move, const Board& board, const std::vector<Move>& move_history) const;
        bool has_castling_pieces_moved(const std::vector<Move>& move_history, int starting_row, int king_x, int rook_x) const;
        bool passes_through_check_when_castling(const Move& castling_move, const Board& board, const std::vector<Move>& move_history) const;
};

#endif
