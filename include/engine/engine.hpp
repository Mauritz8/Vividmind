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
        Engine(Board& board, BoardHelper& board_helper);

        Move iterative_deepening_search(int allocated_time_ms);
        int search_root(int depth, int time_left);
        int search_root(int depth);
        void divide(int depth);

    private:
        Board& board;
        BoardHelper& board_helper;
        MoveGenerator move_gen;
        GameOverDetector game_over_detector;

        Move best_move;
        int depth;
        int score;
        int nodes;
        int time;
        std::vector<Move> pv;

        static const int NO_TIME_CONSTRAINT = INT_MAX;
        static const int NO_TIME_LEFT = INT_MIN;

        int search(int depth, int alpha, int beta, int time_left, std::vector<Move>& principal_variation);
        int search_captures(int alpha, int beta);
        int evaluate();
        void show_uci_info() const;
        void move_ordering(std::vector<Move>& legal_moves, int current_depth) const;
};


#endif
