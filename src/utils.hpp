#pragma once

#include "defs.hpp"
#include <vector>

Color get_opposite_color(Color color);
char get_char_representation(PieceType piece_type);
std::vector<std::string> str_split(std::string_view str, char delim);
