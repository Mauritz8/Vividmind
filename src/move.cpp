#include "move.hpp"
#include "defs.hpp"
#include "fmt/core.h"
#include "utils.hpp"

Move::Move(int start, int end) : start(start), end(end), move_type(NORMAL) {}

Move::Move(int start, int end, MoveType move_type)
    : start(start), end(end), move_type(move_type) {}

Move::Move(int start, int end, PieceType promotion_piece)
    : start(start), end(end), move_type(PROMOTION),
      promotion_piece(promotion_piece) {}

Move::Move(const Move &move)
    : start(move.start), end(move.end), move_type(move.move_type),
      promotion_piece(move.promotion_piece) {}

Move &Move::operator=(const Move &move) {
  this->start = move.start;
  this->end = move.end;
  this->move_type = move.move_type;
  this->promotion_piece = move.promotion_piece;
  return *this;
}

bool Move::operator==(const Move &move) const {
  return move.start == start && move.end == end;
}

std::string Move::to_uci_notation() const {
  std::string promotion_piece_str =
      promotion_piece
          ? std::string(1, tolower(get_char_representation(*promotion_piece)))
          : "";

  return fmt::format("{}{}{}", SQUARES.at(start), SQUARES.at(end),
                     promotion_piece_str);
}
