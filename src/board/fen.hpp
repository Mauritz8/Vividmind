#ifndef FEN_HPP
#define FEN_HPP

#include "board.hpp"
#include <string_view>

namespace fen {
Board get_position(std::string_view fen);
} // namespace fen

#endif
