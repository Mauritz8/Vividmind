#include "board.hpp"

bool Board::is_insufficient_material() const {
  for (const std::vector<Piece> &pieces : game_state.pieces) {
    for (Piece piece : pieces) {
      if (piece.piece_type == PAWN || piece.piece_type == ROOK ||
          piece.piece_type == QUEEN) {
        return false;
      }
    }
  }
  return true;
}

// this will only check if the same position repeats twice
// because otherwise it will do one repetition in a lot of positions,
// before it realizes that it has to do something else
bool Board::is_threefold_repetition() const {
  Board old_board = *this;
  const int history_size = history.size();
  for (int _ = 0; _ < history_size - 1; _++) {

    // if there has been a capture or a pawn move
    // it's impossible to reach the same position again
    if (old_board.game_state.halfmove_clock == 0) {
      return false;
    }

    old_board.undo();
    if (*this == old_board) {
      return true;
    }
  }
  return false;
}

bool Board::is_draw_by_fifty_move_rule() const {
  return game_state.halfmove_clock > 100;
}
