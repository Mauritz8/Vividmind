#ifndef BOARD_HELPER_H
#define BOARD_HELPER_H

#include "board.h"
#include "move.h"
#include "piece.h"
#include "pos.h"


class BoardHelper {
    public:
        BoardHelper(Board& board);

        bool is_occupied_by_color(Pos pos, Color color) const;
        bool is_clear_line(Pos pos1, Pos pos2) const;
        bool is_clear_diagonal(Pos pos1, Pos pos2) const;
        Pos get_king_square(Color color) const;
        void make_appropriate(const Move& move) const;
        void undo_appropriate(const Move& move) const;

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
