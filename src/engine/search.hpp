#pragma once

#include <atomic>
#include <vector>

#include "board/board.hpp"
#include "engine/search_defs.hpp"
#include "move.hpp"

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
                 std::vector<Move> &principal_variation,
                 const std::optional<Move> &best_move_prev_depth);
  int quiescence(int alpha, int beta, std::vector<Move> &principal_variation);
  bool is_terminate();
  void sort_moves(std::vector<Move> &moves);
  int get_move_score(const Move &move);
};
