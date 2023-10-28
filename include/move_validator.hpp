#ifndef MOVE_VALIDATOR_H
#define MOVE_VALIDATOR_H

#include "board.hpp"
#include "board_helper.hpp"
#include "move.hpp"


class MoveValidator {
    public:
        MoveValidator(const Board& board, BoardHelper& board_helper);

        bool is_valid_rook_move(const Move& move) const;
        bool is_valid_bishop_move(const Move& move) const;
        bool is_valid_queen_move(const Move& move) const;
        bool is_valid_knight_move(const Move& move) const;
        bool is_valid_king_move(const Move& move) const;
        bool is_valid_castling_move(const Move& move) const;
        bool is_valid_pawn_move(const Move& move) const;
        bool is_valid_pawn_move_threat(const Move& move) const;
        bool is_valid_en_passant(const Move& pawn_capture) const;
        bool is_promotion_move(const Move& pawn_move) const;

    private:
        const Board& board;
        BoardHelper& board_helper;
};

#endif
