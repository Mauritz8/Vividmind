#include "board.hpp"
#include "fen.hpp"

std::unique_ptr<Board> Board::get_starting_position() {
  return fen::get_position(STARTING_POSITION_FEN);
}

bool Board::is_draw() const {
  return is_insufficient_material() || is_threefold_repetition() ||
         is_draw_by_fifty_move_rule();
}
