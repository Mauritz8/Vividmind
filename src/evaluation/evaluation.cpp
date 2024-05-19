#include "evaluation.hpp"

#include "board.hpp"
#include "piece.hpp"
#include "utils.hpp"

// the evaluation function is very straight forward
// it only considers material and piece-square tables
int Board::evaluate() const {

  // the evaluation from white's point of view
  int evaluation = 0;

  // calculate the material balance from white's point of view
  const int white_material = game_state.material.at(WHITE);
  const int black_material = game_state.material.at(BLACK);
  evaluation += white_material - black_material;

  // calculate the difference between the piece-square table values
  // also from white's point of view
  const int white_psqt = game_state.psqt.at(WHITE);
  const int black_psqt = game_state.psqt.at(BLACK);
  evaluation += white_psqt - black_psqt;

  // always return the score from the view of the player to move
  // so a positive value always means a good position
  // and a negative value means a bad position
  if (game_state.player_to_move == BLACK) {
    return -evaluation;
  }
  return evaluation;
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

int Board::get_psqt_score_static(Piece piece) {
  const int square =
      piece.color == WHITE ? piece.pos : get_mirrored_pos(piece.pos);
  switch (piece.piece_type) {
  case KING: {
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
