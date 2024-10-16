#include "evaluation.hpp"

#include "board/board.hpp"
#include "board/piece.hpp"
#include "utils.hpp"
#include <algorithm>
#include <numeric>

int evaluate(const Board &board) {
  const int evaluation = board.get_material(WHITE) - board.get_material(BLACK) +
                         board.get_psqt(WHITE) - board.get_psqt(BLACK);
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

int get_psqt_score(const Piece &piece, bool is_lone_king, bool is_endgame) {
  const int square =
      piece.color == WHITE ? piece.pos : get_mirrored_pos(piece.pos);
  switch (piece.piece_type) {
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

int Board::get_psqt_score(const Piece &piece) const {
  return ::get_psqt_score(piece, is_lone_king(piece.color), is_endgame());
}

bool Board::is_lone_king(Color color) const {
  return game_state.pieces.at(color).size() == 1;
}

bool Board::is_endgame() const {
  return game_state.material.at(WHITE) - KING_VALUE < 1500 &&
         game_state.material.at(BLACK) - KING_VALUE < 1500;
}
