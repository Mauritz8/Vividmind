#ifndef GAME_OVER_DETECTOR_H
#define GAME_OVER_DETECTOR_H

#include <vector>

#include "board.h"
#include "move.h"
#include "move_generator.h"


class GameOverDetector {
    public:
        GameOverDetector(const Board& board, const MoveGenerator& move_generator);

        bool is_checkmate(std::vector<Move>& legal_moves) const;
        bool is_draw(std::vector<Move>& legal_moves) const;

    private:
        const Board& board;
        const MoveGenerator& move_gen;

        bool is_insufficient_material() const;
        bool is_stalemate(const std::vector<Move>& legal_moves) const;
        bool is_threefold_repetition() const;
};

#endif
