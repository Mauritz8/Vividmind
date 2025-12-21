#include "evaluation.hpp"

#include "board/board.hpp"

int evaluate(const Board &board) {
  const int material = board.get_material(WHITE) - board.get_material(BLACK);
  const int psqt = board.get_psqt(WHITE) - board.get_psqt(BLACK);
  const int doubled_pawns =
      board.get_doubled_pawns(WHITE) - board.get_doubled_pawns(BLACK);

  const int evaluation = material + psqt - doubled_pawns * 30;
  return board.get_player_to_move() == BLACK ? -evaluation : evaluation;
}

int get_psqt_score(PieceType piece_type, int pos, Color color,
                   bool is_lone_king, bool is_endgame) {
  const int square = color == WHITE ? pos : FLIPPED_BOARD.at(pos);
  if (piece_type == KING) {
    if (is_lone_king) {
      return KING_MATE.at(square);
    }
    if (is_endgame) {
      return KING_ENDGAME_PSQT.at(square);
    }
  }
  return PSQTs.at(piece_type).at(square);
}
