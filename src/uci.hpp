#pragma once

#include <sstream>
#include <string>
#include <vector>

#include "board.hpp"
#include "search.hpp"

class UCI {
public:
  UCI(Board &board);

  void process(const std::string &input);
  static std::string show(const SearchSummary &search_summary);
  static std::string bestmove(const Move &move);

private:
  Board &board;
  Search search;

  static const int MOVE_OVERHEAD = 50;

  std::string get_fen(const std::vector<std::string> &words) const;
  SearchParams get_search_params(const std::vector<std::string> &words) const;

  bool make_move(const std::string &move);
};
