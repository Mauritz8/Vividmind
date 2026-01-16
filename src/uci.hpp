#pragma once

#include <forward_list>
#include <string>

#include "engine/command.hpp"
#include "move.hpp"

struct SearchSummary {
  int depth;
  int seldepth;
  int score;
  long long nodes;
  long long time;
  std::forward_list<Move> pv;
};

namespace uci {
Command process(const std::string &input);
std::string show(const SearchSummary &search_summary);
std::string bestmove(const Move &move);
}; // namespace uci
