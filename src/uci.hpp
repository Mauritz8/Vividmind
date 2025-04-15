#pragma once

#include <string>

#include "engine/command.hpp"
#include "engine/search_defs.hpp"

namespace uci {
Command process(const std::string &input);
std::string show(const SearchSummary &search_summary);
std::string bestmove(const Move &move);
}; // namespace uci
