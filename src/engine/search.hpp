#pragma once

#include <atomic>
#include <chrono>
#include <forward_list>
#include <map>
#include <unordered_set>

#include "board/board.hpp"
#include "move.hpp"

struct SearchInfo {
  int seldepth;
  long nodes;
  std::map<int, std::unordered_set<Move, Move::HashFunction>> killer_moves;
};

struct SearchParams {
  int depth;
  std::forward_list<Move> principal_variation;
  int allocated_time;
  std::chrono::time_point<std::chrono::high_resolution_clock> start_time;
  const std::atomic<bool> &stop;
};

const int DRAW = 0;
const int CHECKMATE = 50000;
const int CHECKMATE_THRESHOLD = 49000;

namespace search {
void iterative_deepening_search(Board &board, int depth, int allocated_time,
                                std::atomic<bool> &stop);
};
