#pragma once

#include "defs.hpp"

class Piece {
public:
  PieceType piece_type;
  Color color;
  int pos;

  Piece(PieceType piece_type, Color color, int pos);

  bool operator==(Piece piece) const;
  bool operator!=(Piece piece) const;
};
