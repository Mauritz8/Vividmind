#ifndef MOVE_VALIDATOR_HPP
#define MOVE_VALIDATOR_HPP

#include "board.hpp"
#include "move.hpp"


class MoveValidator {
    public:
        MoveValidator(const Board& board);

        bool is_valid_en_passant(const Move& pawn_capture) const;
        bool is_clear_line(int pos1, int pos2) const;

    private:
        const Board& board;
};

#endif
