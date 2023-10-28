#ifndef ENGINE_H
#define ENGINE_H

#include "board.hpp"
#include "board_helper.hpp"
#include "move.hpp"
#include "move_generator.hpp"
#include "game_over_detector.hpp"


class Engine {
    public:
        Engine(Board& board, BoardHelper& board_helper);

        Move get_best_move(int depth);
        Move iterative_deepening_search(int allocated_time_ms);
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
