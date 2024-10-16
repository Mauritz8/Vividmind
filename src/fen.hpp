#pragma once

#include "board.hpp"
#include <memory>
#include <string_view>

namespace fen {
std::unique_ptr<Board> get_position(std::string_view fen);
} // namespace fen
