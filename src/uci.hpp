#pragma once

#include <string>

#include "board/board.hpp"
#include "search/search.hpp"

namespace uci {
void process(const std::string &input, Board &board);
std::string show(const SearchSummary &search_summary);
std::string bestmove(const Move &move);
}; // namespace uci
