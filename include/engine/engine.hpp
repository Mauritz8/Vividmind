#ifndef ENGINE_H
#define ENGINE_H

#include <chrono>
#include <climits>
#include <vector>

#include "board.hpp"
#include "move.hpp"
#include "move_generator.hpp"
#include "game_over_detector.hpp"


enum SearchMode {DEPTH, MOVE_TIME, INFINITE};

struct GameTime {
    int wtime;
    int btime;
    int winc;
    int binc;
    int moves_to_go;
};

struct SearchParams {
    int depth;
    int allocated_time;
    GameTime game_time;
    SearchMode search_mode;
};

// all the collected info during a search will be stored in this struct
struct SearchInfo {
    std::chrono::time_point<std::chrono::high_resolution_clock> start_time;
    int depth;
    long nodes;
    bool is_terminated;

    int time_elapsed() const;
};

struct SearchSummary {
    int depth;
    int score;
    long nodes;
    long time;
    std::vector<Move> pv;
};

class Engine {
    public:
        SearchParams search_params;

        static const int MAX_PLY = 100;

        Engine(Board& board);

        void iterative_deepening_search();
        void divide(int depth);
        int get_allocated_time();

    private:
        Board& board;
        MoveGenerator move_gen;
        GameOverDetector game_over_detector;

        SearchInfo search_info;

        static const int DRAW = 0;
        static const int CHECKMATE = 50000;
        static const int CHECKMATE_THRESHOLD = 49000;

        int search(int depth, int alpha, int beta, std::vector<Move>& principal_variation);
        int search_captures(int alpha, int beta);
        int evaluate();
        void show_uci_info(const SearchSummary& search_summary) const;
        void check_termination();
};


#endif
