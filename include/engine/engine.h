#ifndef ENGINE_H
#define ENGINE_H

#include "board.h"
#include "board_helper.h"
#include "move_generator.h"
#include "game_over_detector.h"


class Engine {
    public:
        Engine(Board& board, BoardHelper& board_helper);

        Move get_best_move(int depth);
        Move iterative_deepening_search(int max_depth);
        void divide(int depth);

    private:
        Board& board;
        BoardHelper& board_helper;
        MoveGenerator move_gen;
        GameOverDetector game_over_detector;

        int current_depth;
        int evaluation;
        int nodes_searched;
        int time;

        int search(int depth, int alpha, int beta);
        int search_captures(int alpha, int beta);
        int evaluate();
        void show_uci_info() const;
};


#endif
