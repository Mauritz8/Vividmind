#include <algorithm>
#include <cassert>

#include "evaluation/evaluation.hpp"
#include "move_sort.hpp"

static int mvv_lva_move_score(const Move &move,
                              const std::optional<Move> &best_move_prev_depth,
                              const Board &board) {
  if (best_move_prev_depth.has_value() &&
      move == best_move_prev_depth.value()) {
    return QUEEN_VALUE;
  }

  const std::optional<PieceType> start_piece = board.get_piece_type(move.start);
  const std::optional<PieceType> end_piece = board.get_piece_type(move.end);

  // score non-capture moves lower than captures
  if (!end_piece) {
    return -QUEEN_VALUE;
  }
  return PIECE_VALUES.at(end_piece.value()) -
         PIECE_VALUES.at(start_piece.value());
}

void sort_moves(std::vector<Move> &moves,
                const std::optional<Move> &best_move_prev_depth,
                const Board &board) {
  auto sort_mvv_lva = [&](Move i, Move j) {
    return mvv_lva_move_score(i, best_move_prev_depth, board) >
           mvv_lva_move_score(j, best_move_prev_depth, board);
  };
  std::sort(moves.begin(), moves.end(), sort_mvv_lva);
}
