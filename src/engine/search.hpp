#pragma once

#include <vector>

#include "board.hpp"
#include "move.hpp"
#include "engine/search_defs.hpp"

class Search {
public:
  const SearchParams params;
  SearchInfo info;

  Search(std::unique_ptr<Board> &board, SearchParams &params,
         bool &stop);

  void iterative_deepening_search();

private:
  std::unique_ptr<Board> &board;
  bool &stop;

  int alpha_beta(int depth, int alpha, int beta,
                 std::vector<Move> &principal_variation);
  int quiescence(int alpha, int beta, std::vector<Move> &principal_variation);
  bool is_terminate();
  void sort_moves(std::vector<Move> &moves);
  int get_move_score(const Move &move);
};
