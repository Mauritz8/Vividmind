#pragma once

#include <vector>

#include "board.hpp"
#include "move.hpp"
#include "search/defs.hpp"

class Search {
public:
  SearchParams params;
  SearchInfo info;

  Search(Board &board);

  void iterative_deepening_search();
  int calc_allocated_time() const;

private:
  Board &board;

  int alpha_beta(int depth, int alpha, int beta,
                 std::vector<Move> &principal_variation);
  int quiescence(int alpha, int beta, std::vector<Move> &principal_variation);
  bool is_terminate();
  void sort_moves(std::vector<Move> &moves);
  int get_move_score(const Move &move);
};
