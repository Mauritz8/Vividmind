#include "move_gen.hpp"

std::vector<Move> MoveGenerator::get_pawn_captures(const Piece &piece) const {
  const int direction = piece.color == BLACK ? 1 : -1;
  const std::array<int, 2> movements = {9 * direction, 11 * direction};

  std::vector<Move> moves;
  moves.reserve(2);
  for (int movement : movements) {
    const int end = mailbox.at(mailbox64.at(piece.pos) + movement);
    if (end == -1) {
      continue;
    }

    const Square &square = board.squares.at(end);
    if (square.piece && square.piece->color != piece.color) {
      moves.push_back(Move(piece.pos, end));
    } else if (is_valid_en_passant(end)) {
      Move move = Move(piece.pos, end);
      move.move_type = EN_PASSANT;
      moves.push_back(move);
    }
  }
  return moves;
}

bool MoveGenerator::is_valid_en_passant(int capture_pos) const {
  if (!board.game_state.en_passant_square.has_value()) {
    return false;
  }
  return capture_pos == board.game_state.en_passant_square.value();
}
