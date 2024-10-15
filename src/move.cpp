#include "move.hpp"
#include "board/defs.hpp"
#include "defs.hpp"
#include "fmt/core.h"
#include "utils.hpp"

Move::Move(int start, int end) {
  this->start = start;
  this->end = end;
  this->move_type = NORMAL;
}

Move::Move(const Move &move) {
  this->start = move.start;
  this->end = move.end;
  this->move_type = move.move_type;
  this->promotion_piece = move.promotion_piece;
}

Move Move::operator=(const Move &move) {
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
