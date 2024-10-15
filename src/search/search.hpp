#pragma once

#include <vector>

#include "../board/board.hpp"
#include "move.hpp"
#include "search/defs.hpp"

class Search {
public:
  const SearchParams params;
  SearchInfo info;

  Search(Board &board, SearchParams &params);

  void iterative_deepening_search();

private:
  Board &board;

  int alpha_beta(int depth, int alpha, int beta,
                 std::vector<Move> &principal_variation);
  int quiescence(int alpha, int beta, std::vector<Move> &principal_variation);
  bool is_terminate();
  void sort_moves(std::vector<Move> &moves);
  int get_move_score(const Move &move);
};
