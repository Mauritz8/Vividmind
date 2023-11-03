#ifndef ENGINE_H
#define ENGINE_H

#include <climits>
#include <vector>

#include "board.hpp"
#include "board_helper.hpp"
#include "move.hpp"
#include "move_generator.hpp"
#include "game_over_detector.hpp"


struct SearchParams {
    int wtime;
    int btime;
    int winc;
    int binc;
    int moves_to_go;
};

struct SearchResult {
    int depth;
    int score;
    long nodes;
    long time;
    std::vector<Move> pv;
};

class Engine {
    public:
        SearchParams search_params;

        Engine(Board& board, BoardHelper& board_helper);

        void iterative_deepening_search_depth(int search_depth);
        void iterative_deepening_search_time(int allocated_time_ms);
        void iterative_deepening_search_infinite();
        void divide(int depth);
        int get_allocated_time();

    private:
        Board& board;
        BoardHelper& board_helper;
        MoveGenerator move_gen;
        GameOverDetector game_over_detector;

        SearchResult search_result;

        static const int NO_CONSTRAINT = INT_MAX;
        static const int NO_TIME_LEFT = INT_MIN;
        static const int MOVE_OVERHEAD = 50;
        static const int DRAW = 0;
        static const int CHECKMATE = 50000;
        static const int CHECKMATE_THRESHOLD = 49000;
        static const int ALPHA_INITIAL_VALUE = -CHECKMATE;

        void iterative_deepening_search(int search_depth, int allocated_time_ms);
        int search(int depth, int alpha, int beta, int time_left, std::vector<Move>& principal_variation, bool last_was_nullmove);
        int search_captures(int alpha, int beta, int time_left);
        int evaluate();
        void show_uci_info() const;
        void move_ordering(std::vector<Move>& legal_moves, int current_depth) const;
};


#endif
