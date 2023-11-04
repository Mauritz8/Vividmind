#ifndef GAME_OVER_DETECTOR_HPP
#define GAME_OVER_DETECTOR_HPP

#include <vector>

#include "board.hpp"
#include "move.hpp"
#include "move_generator.hpp"


class GameOverDetector {
    public:
        GameOverDetector(Board& board, const MoveGenerator& move_generator);

        bool is_checkmate(std::vector<Move>& legal_moves) const;
        bool is_insufficient_material() const;
        bool is_threefold_repetition() const;

    private:
        Board& board;
        const MoveGenerator& move_gen;

};

#endif
