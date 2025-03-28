#pragma once

#include <chrono>
#include <vector>

#include "move.hpp"

enum SearchMode { DEPTH, MOVE_TIME, INFINITE };

struct SearchParams {
  int depth;
  int allocated_time;
  SearchMode search_mode;

  SearchParams();
};

// all the collected info during a search will be stored in this struct
struct SearchInfo {

  // keep track of when the search started,
  // so that it can stop if the allocated time runs out
  std::chrono::time_point<std::chrono::high_resolution_clock> start_time;

  int depth;
  int ply_from_root;
  int seldepth;
  long nodes;
  bool is_terminated;

  SearchInfo();

  int time_elapsed() const;
};

struct SearchSummary {
  int depth;
  int seldepth;
  int score;
  long long nodes;
  long long time;
  std::vector<Move> pv;
};

const int MAX_PLY = 100;
const int DRAW = 0;
const int CHECKMATE = 50000;
const int CHECKMATE_THRESHOLD = 49000;
