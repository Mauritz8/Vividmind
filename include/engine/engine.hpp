#ifndef ENGINE_H
#define ENGINE_H

#include <climits>
#include <vector>

#include "board.hpp"
#include "board_helper.hpp"
#include "move.hpp"
#include "move_generator.hpp"
#include "game_over_detector.hpp"


class Engine {
    public:
        Move best_move;

        Engine(Board& board, BoardHelper& board_helper);

        void iterative_deepening_search_depth(int search_depth);
        void iterative_deepening_search_time(int allocated_time_ms);
        void iterative_deepening_search_infinite();
        void divide(int depth);

    private:
        Board& board;
        BoardHelper& board_helper;
        MoveGenerator move_gen;
        GameOverDetector game_over_detector;

        int depth;
        int score;
        int nodes;
        int time;
        std::vector<Move> pv;

        static const int NO_CONSTRAINT = INT_MAX;
        static const int NO_TIME_LEFT = INT_MIN;

        void iterative_deepening_search(int search_depth, int allocated_time_ms);
        int search_root(int depth, int time_left);
        int search(int depth, int alpha, int beta, int time_left, std::vector<Move>& principal_variation);
        int search_captures(int alpha, int beta, int time_left);
        int evaluate();
        void show_uci_info() const;
        void move_ordering(std::vector<Move>& legal_moves, int current_depth) const;
};


#endif
