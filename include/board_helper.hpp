#ifndef BOARD_HELPER_H
#define BOARD_HELPER_H

#include "board.hpp"
#include "move.hpp"
#include "piece.hpp"


class BoardHelper {
    public:
        BoardHelper(Board& board);

        bool is_occupied_by_color(int pos, Color color) const;
        bool is_clear_line(int pos1, int pos2) const;
        bool is_clear_diagonal(int pos1, int pos2) const;
        int get_king_square(Color color) const;
        void make_appropriate(const Move& move) const;
        void undo_appropriate() const;
        void make_null_move() const;
        void undo_null_move() const;

    private:
        Board& board;

        void make(const Move& move) const;
        void undo(const Move& move) const;
        void update_castling_rights(const Move& move) const;
        Move get_castling_rook_move(const Move& move) const;
        void make_castling(const Move& move) const;
        void undo_castling(const Move& move) const;
        void make_en_passant(const Move& move) const;
        void undo_en_passant(const Move& move) const;
        void make_promotion(const Move& move) const;
        void undo_promotion(const Move& move) const;
        void make_pawn_two_squares_forward(const Move& move) const;
        void undo_pawn_two_squares_forward(const Move& move) const;
};

#endif
