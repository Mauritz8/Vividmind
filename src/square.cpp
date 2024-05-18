#include "square.hpp"

Square::Square(int pos) { this->pos = pos; }

Square::Square(int pos, std::optional<Piece> piece) {
  this->pos = pos;
  this->piece = piece;
}

bool Square::is_occupied_by(Color color) const {
  return piece && piece->color == color;
}
