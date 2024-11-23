#include "evaluation.hpp"

#include "piece.hpp"
#include "utils.hpp"

int evaluate(const std::unique_ptr<Board> &board) {
  const int evaluation = board->material(WHITE) - board->material(BLACK) +
                         board->psqt(WHITE) - board->psqt(BLACK);
  return board->player_to_move() == BLACK ? -evaluation : evaluation;
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

int get_piece_value(PieceType piece_type) {
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

int get_psqt_score(PieceType piece_type, int pos, Color color,
                   bool is_lone_king, bool is_endgame) {
  const int square = color == WHITE ? pos : get_mirrored_pos(pos);
  switch (piece_type) {
  case KING: {
    if (is_lone_king) {
      return KING_MATE.at(square);
    }
    if (is_endgame) {
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
