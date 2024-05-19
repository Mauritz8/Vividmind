#ifndef UTILS_HPP
#define UTILS_HPP

#include "defs.hpp"
#include <vector>

Color get_opposite_color(Color color);
char get_char_representation(PieceType piece_type);
PieceType get_piece_type(char char_representation);
int get_mirrored_pos(int pos);
std::vector<std::string> str_split(std::string_view str, char delim);

#endif
