#ifndef ENGINE_H
#define ENGINE_H

#include "board.h"
#include "move_generator.h"
#include "game_over_detector.h"


class Engine {
    public:
        Engine(Board& board);

        Move get_best_move(int depth);
        void divide(int depth);

    private:
        Board& board;
        MoveGenerator move_gen;
        GameOverDetector game_over_detector;

        int search(int depth, int alpha, int beta);
        int search_captures(int alpha, int beta);
        int evaluate() const;
};


#endif
