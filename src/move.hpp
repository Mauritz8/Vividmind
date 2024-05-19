#pragma once

#include <optional>
#include <string>

#include "defs.hpp"

enum MoveType {
  NORMAL,
  CASTLING,
  PROMOTION,
  EN_PASSANT,
  PAWN_TWO_SQUARES_FORWARD
};

class Move {
public:
  int start;
  int end;
  MoveType move_type;
  std::optional<PieceType> promotion_piece;

  Move(){};
  Move(int start, int end);
  Move(const Move &move);

  Move operator=(const Move &move);
  bool operator==(const Move &move) const;

  std::string to_uci_notation() const;
};
