#include "utils.hpp"

#include <stdexcept>
#include <vector>

Color get_opponent(Color color) { return color == WHITE ? BLACK : WHITE; }

char get_char_representation(PieceType piece_type) {
  switch (piece_type) {
  case PAWN:
    return 'p';
  case KNIGHT:
    return 'N';
  case BISHOP:
    return 'B';
  case ROOK:
    return 'R';
  case QUEEN:
    return 'Q';
  case KING:
    return 'K';
  }
  throw std::invalid_argument("invalid piece type");
}

std::vector<std::string> str_split(std::string_view str, char delim) {
  std::vector<std::string> substrings;
  std::string substr = "";
  for (char ch : str) {
    if (ch == delim) {
      substrings.push_back(substr);
      substr = "";
    } else {
      substr += ch;
    }
  }
  if (substr != "")
    substrings.push_back(substr);
  return substrings;
}
