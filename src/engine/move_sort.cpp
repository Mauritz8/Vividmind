#include <algorithm>
#include <cassert>

#include "evaluation/evaluation.hpp"
#include "move_sort.hpp"

static int
move_score(const Move &move, const std::optional<Move> &best_move_prev_depth,
           std::unordered_set<Move, Move::HashFunction> &killer_moves,
           const Board &board) {
  if (best_move_prev_depth.has_value() &&
      move == best_move_prev_depth.value()) {
    return QUEEN_VALUE;
  }

  if (killer_moves.contains(move)) {
    return 0;
  }

  const std::optional<PieceType> start_piece = board.get_piece_type(move.start);
  const std::optional<PieceType> end_piece = board.get_piece_type(move.end);

  // score non-capture moves lower than captures
  if (!end_piece.has_value()) {
    return -QUEEN_VALUE;
  }
  return PIECE_VALUES.at(end_piece.value()) -
         PIECE_VALUES.at(start_piece.value());
}

void sort_moves(std::vector<Move> &moves,
                const std::optional<Move> &best_move_prev_depth,
                std::unordered_set<Move, Move::HashFunction> &killer_moves,
                const Board &board) {
  std::sort(moves.begin(), moves.end(), [&](Move i, Move j) {
    return move_score(i, best_move_prev_depth, killer_moves, board) >
           move_score(j, best_move_prev_depth, killer_moves, board);
  });
}
