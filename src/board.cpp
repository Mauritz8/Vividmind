#include "board.hpp"
#include "fen.hpp"

std::unique_ptr<Board> Board::get_starting_position() {
  return fen::get_position(STARTING_POSITION_FEN);
}
