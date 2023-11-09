#ifndef SEARCH_DEFS_HPP
#define SEARCH_DEFS_HPP

#include <chrono>
#include <vector>

#include "move.hpp"


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

    SearchParams();
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


const int MAX_PLY = 100;
const int DRAW = 0;
const int CHECKMATE = 50000;
const int CHECKMATE_THRESHOLD = 49000;

#endif
