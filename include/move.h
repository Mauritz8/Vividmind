#ifndef MOVE_H
#define MOVE_H

#include <optional>
#include <string>
#include <vector>

#include "board.h"
#include "piece.h"
#include "pos.h"
#include "square.h"


class Move {
    public:
        Pos start;
        Pos end;
        bool is_castling_move;
        bool is_promotion;
        std::optional<PieceType> promotion_piece;
        bool is_en_passant;
        bool is_pawn_two_squares_forward;

        Move(int start_x, int start_y, int end_x, int end_y);
        Move(Pos start, Pos end);
        Move(const Square& start, const Square& end);
        static Move get_from_uci_notation(const std::string& uci_notation, const Board& board);
        Move(const Move& move);

        Move operator=(const Move& move);
        bool operator==(const Move& move) const;

        void make_appropriate(Board& board) const;
        void undo_appropriate(Board& board) const;
        std::string to_uci_notation() const;

    private:
        void make(Board& board) const;
        void undo(Board& board) const;
        void update_castling_rights(Board& board) const;
        Move get_castling_rook_move(const Board& board) const;
        void make_castling(Board& board) const;
        void undo_castling(Board& board) const;
        void make_en_passant(Board& board) const;
        void undo_en_passant(Board& board) const;
        void make_promotion(Board& board) const;
        void undo_promotion(Board& board) const;
        void make_pawn_two_squares_forward(Board& board) const;
        void undo_pawn_two_squares_forward(Board& board) const;
};

#endif
