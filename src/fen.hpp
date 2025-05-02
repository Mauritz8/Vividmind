#pragma once

#include "bitboards_board/bitboards_board.hpp"
#include <string_view>

namespace fen {
BitboardsBoard get_position(std::string_view fen);
} // namespace fen
