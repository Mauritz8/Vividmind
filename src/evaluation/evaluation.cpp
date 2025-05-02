#include "evaluation.hpp"

#include "board/board.hpp"
#include "piece.hpp"

int evaluate(const Board &board) {
  const int material = board.get_material(WHITE) - board.get_material(BLACK);
  const int psqt = board.get_psqt(WHITE) - board.get_psqt(BLACK);
  const int doubled_pawns =
      board.get_doubled_pawns(WHITE) - board.get_doubled_pawns(BLACK);

  const int evaluation = material + psqt - doubled_pawns * 30;
  return board.get_player_to_move() == BLACK ? -evaluation : evaluation;
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

int get_mirrored_pos(int pos) {
  const int x = pos % 8;
  const int y = pos / 8;
  return x + (7 - y) * 8;
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
