#pragma once

#include <atomic>
#include <string>

#include "engine/search_defs.hpp"

namespace uci {
void process(const std::string &input, int write_descriptor,
             std::atomic<bool> &stop);
std::string show(const SearchSummary &search_summary);
std::string bestmove(const Move &move);
}; // namespace uci
