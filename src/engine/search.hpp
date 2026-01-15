#pragma once

#include <atomic>
#include <chrono>
#include <map>
#include <unordered_set>

#include "board/board.hpp"
#include "move.hpp"

enum SearchMode { DEPTH, MOVE_TIME, INFINITE };

struct SearchParams {
  SearchMode search_mode;
  int depth;
  int allocated_time;
};

struct SearchInfo {
  std::chrono::time_point<std::chrono::high_resolution_clock> start_time;
  int depth;
  int ply_from_root;
  int seldepth;
  long nodes;
  bool is_terminated;
  std::optional<Move> best_move;
  std::map<int, std::unordered_set<Move, Move::HashFunction>> killer_moves;
  int quiescence_plies;
};

const int MAX_PLY = 100;
const int DRAW = 0;
const int CHECKMATE = 50000;
const int CHECKMATE_THRESHOLD = 49000;

class Search {
public:
  const SearchParams params;
  SearchInfo info;

  Search(Board &board, SearchParams &params, std::atomic<bool> &stop);

  void iterative_deepening_search();

private:
  Board &board;
  std::atomic<bool> &stop;

  int alpha_beta(int depth, int alpha, int beta,
                 std::vector<Move> &principal_variation);
  int quiescence(int alpha, int beta, std::vector<Move> &principal_variation);
  bool is_terminate();
};
