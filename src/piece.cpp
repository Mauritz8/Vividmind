#include "piece.hpp"

Piece::Piece(PieceType piece_type, Color color, int pos) {
  this->piece_type = piece_type;
  this->color = color;
  this->pos = pos;
}

bool Piece::operator==(Piece piece) const {
  return piece.pos == pos && piece.color == color &&
         piece.piece_type == piece_type;
}

bool Piece::operator!=(Piece piece) const { return !(*this == piece); }
