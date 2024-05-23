#include "evaluation.hpp"

#include "board.hpp"
#include "piece.hpp"
#include "utils.hpp"

int Board::evaluate() const {
  const int white_material = game_state.material.at(WHITE);
  const int black_material = game_state.material.at(BLACK);
  const int white_psqt = game_state.psqt.at(WHITE);
  const int black_psqt = game_state.psqt.at(BLACK);

  const int evaluation =
      white_material - black_material + white_psqt - black_psqt;
  return game_state.player_to_move == BLACK ? -evaluation : evaluation;
}

int Piece::get_value() const {
  switch (piece_type) {
  case KING:
    return KING_VALUE;
  case QUEEN:
    return QUEEN_VALUE;
  case ROOK:
    return ROOK_VALUE;
  case BISHOP:
    return BISHOP_VALUE;
  case KNIGHT:
    return KNIGHT_VALUE;
  case PAWN:
    return PAWN_VALUE;
  }
}

int Board::get_psqt_score(const Piece &piece) const {
  const int square =
      piece.color == WHITE ? piece.pos : get_mirrored_pos(piece.pos);
  switch (piece.piece_type) {
  case KING: {
    if (is_lone_king(piece.color)) {
      return KING_MATE.at(square);
    }
    if (is_endgame()) {
      return KING_ENDGAME_PSQT.at(square);
    }
    return KING_PSQT.at(square);
  }
  case QUEEN:
    return QUEEN_PSQT.at(square);
  case ROOK:
    return ROOK_PSQT.at(square);
  case BISHOP:
    return BISHOP_PSQT.at(square);
  case KNIGHT:
    return KNIGHT_PSQT.at(square);
  case PAWN:
    return PAWN_PSQT.at(square);
  }
}

bool Board::is_lone_king(Color color) const {
  return pieces.at(color).size() == 1;
}

bool Board::is_endgame() const {
  return game_state.material.at(WHITE) - KING_VALUE < 1500 &&
         game_state.material.at(BLACK) - KING_VALUE < 1500;
}
