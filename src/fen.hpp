#pragma once

#include "mailbox_board/mailbox_board.hpp"
#include <string_view>

namespace fen {
MailboxBoard get_position(std::string_view fen);
} // namespace fen
