#ifndef ENGINE_H
#define ENGINE_H

#include <climits>
#include <vector>

#include "board.hpp"
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

struct SearchInfo {
    int depth;
    int score;
    long nodes;
    long time;
    std::vector<Move> pv;
    bool is_terminated;
};

class Engine {
    public:
        SearchParams search_params;

        Engine(Board& board);

        void iterative_deepening_search_depth(int search_depth);
        void iterative_deepening_search_time(int allocated_time_ms);
        void iterative_deepening_search_infinite();
        void divide(int depth);
        int get_allocated_time();

    private:
        Board& board;
        MoveGenerator move_gen;
        GameOverDetector game_over_detector;

        SearchInfo search_info;

        static const int NO_CONSTRAINT = 100000;
        static const int MOVE_OVERHEAD = 50;
        static const int DRAW = 0;
        static const int CHECKMATE = 50000;
        static const int CHECKMATE_THRESHOLD = 49000;
        static const int ALPHA_INITIAL_VALUE = -CHECKMATE;

        void iterative_deepening_search(int search_depth, int allocated_time_ms);
        int search(int depth, int alpha, int beta, int time_left, std::vector<Move>& principal_variation);
        int search_captures(int alpha, int beta, int time_left);
        int evaluate();
        void show_uci_info() const;
        void move_ordering(std::vector<Move>& legal_moves, int current_depth) const;
};


#endif
