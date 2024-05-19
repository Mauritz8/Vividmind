#include "utils.hpp"

#include <stdexcept>

Color get_opposite_color(Color color) { return color == WHITE ? BLACK : WHITE; }

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

PieceType get_piece_type(char char_representation) {
  switch (tolower(char_representation)) {
  case 'p':
    return PAWN;
  case 'n':
    return KNIGHT;
  case 'b':
    return BISHOP;
  case 'r':
    return ROOK;
  case 'q':
    return QUEEN;
  case 'k':
    return KING;
  }
  throw std::invalid_argument("invalid piece type");
}

int get_mirrored_pos(int pos) {
  const int x = pos % 8;
  const int y = pos / 8;
  return x + (7 - y) * 8;
}
