#pragma once

#include <string>

#include "mailbox_board/mailbox_board.hpp"
#include "search/search.hpp"

namespace uci {
void process(const std::string &input, MailboxBoard &board);
std::string show(const SearchSummary &search_summary);
std::string bestmove(const Move &move);
}; // namespace uci
